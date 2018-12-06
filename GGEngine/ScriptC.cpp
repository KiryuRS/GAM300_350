#include "stdafx.h"
#include "ScriptC.h"

ScriptC::ScriptC()
{
}

namespace
{
	template<typename T>
	bool AddVariableToMap(const sol::object& item, const std::string& itemName,
		ScriptC& sc)
	{
		if (!item.is<T>())
			return false;
		auto iter = sc.variableData.find(itemName);
		bool updateSerial = iter == sc.variableData.end();
		sc.variableData[itemName].second = item.as<T>();
		sc.variableData[itemName].first = (LUATYPE)MySTL::typeIndex<T, ALLLUATYPES>::value;
		sc.AddSerializable(itemName,
			std::get<T>(sc.variableData[itemName].second), "", nullptr);
		if (updateSerial)
			sc.UpdateSerializables(std::get<T>(sc.variableData[itemName].second));
		return true;
	}
	template<typename T, typename... Args>
	void TestVariableTypes(const sol::object& item, const std::string& name,
		ScriptC& sc)
	{
		if (AddVariableToMap<T>(item, name, sc)) 
			return;
		else if constexpr(sizeof... (Args) > 0)
			TestVariableTypes<Args...>(item, name, sc);
	}
	template<typename T, typename ...Args>
	void BatchSetVariable(ScriptC& sc, LuaScript* script)
	{
		std::string varName = SerialTupleContainer<ALLSERIALIZEDTYPES>::serializedNames[MySTL::typeIndex<T, ALLSERIALIZEDTYPES>::value];
		varName[0] -= 32;
		std::stringstream ss;
		ss << "Set" << varName << "Variable";
		sc.AddCallableFunction(ss.str(), &ScriptC::SetLuaVariable<T>, { "name", "value" }, script);
		if constexpr (sizeof...(Args) > 0)
			BatchSetVariable<Args...>(sc, script);
	}
	template<typename T, typename ...Args>
	void BatchGetVariable(ScriptC& sc, LuaScript* script)
	{
		std::string varName = SerialTupleContainer<ALLSERIALIZEDTYPES>::serializedNames[MySTL::typeIndex<T, ALLSERIALIZEDTYPES>::value];
		varName[0] -= 32;
		std::stringstream ss;
		ss << "Get" << varName << "Variable";
		sc.AddCallableFunction(ss.str(), &ScriptC::GetLuaVariable<T>, script);
		if constexpr (sizeof...(Args) > 0)
			BatchGetVariable<Args...>(sc, script);
	}
	template<typename T, unsigned num = 0>
	void UpdateLuaVariable(sol::state& lua, const T& item)
	{
		if ((unsigned)item.second.first == num)
		{
			lua[item.first] = std::get<std::tuple_element<num, std::tuple<ALLLUATYPES>>::type>(item.second.second);
			return;
		}
		if constexpr ((unsigned)LUATYPE::TYPENUM - num - 1 > 0)
			UpdateLuaVariable< T, num + 1>(lua, item);
	}
	template<typename T, unsigned num = 0>
	void ReflectLuaVariable(const sol::object& lua, T& item)
	{
		using currType = typename std::tuple_element<num, std::tuple<ALLLUATYPES>>::type;
		if ((unsigned)item.first == num)
		{
			std::get<currType>(item.second) = lua.as<currType>();
			return;
		}
		if constexpr ((unsigned)LUATYPE::TYPENUM - num - 1 > 0)
			ReflectLuaVariable< T, num + 1>(lua, item);
	}
	template<typename T, unsigned num = 0>
	void ClearLuaVariable(ScriptC& script, T& item)
	{
		if ((unsigned)item.second.first == num)
			script.RemoveSerializable(std::get<std::tuple_element<num, std::tuple<ALLLUATYPES>>::type>(item.second.second));
		if constexpr ((unsigned)LUATYPE::TYPENUM - num - 1 > 0)
			ClearLuaVariable< T, num + 1>(script, item);
	}
	template<typename T, typename...Args>
	void TestSetVariable(const std::string& name, sol::object& obj, sol::state& state)
	{
		if (obj.is<T>())
		{
			state[name] = obj.as<T>();
			return;
		}
		if constexpr (sizeof...(Args) > 0)
		{
			TestSetVariable<Args...>(name, obj, state);
		}

	}
}

void ScriptC::AddSerializeData(LuaScript* script)
{
	AddSerializable("Scripts", scripts, "", script);
	AddCallableFunction("CallLuaFunction", &ScriptC::CallLuaFunction, script);
	BatchSetVariable<ALLLUATYPES>(*this, script);
	BatchGetVariable<ALLLUATYPES>(*this, script);
	AddCallableFunction("GetVariable", &ScriptC::GetVariable, script);
	AddCallableFunction("SetVariable", &ScriptC::SetVariable, script);
}

bool ScriptC::EditorGUI(Editor& edit)
{
	if (edit.playing)
	{
		ReflectLuaVariables();
		UpdateSerializables();
	}
	if (ImGui::Button("Update Scripts"))
	{
		UpdateComponentData(scripts);
		SetupScripts();
		//UpdateSerializables();
	}
	if (ImGui::Button("New Script"))
	{
		// Create a new popup window that allows the editing of name
		ImGui::OpenPopup("New Script - Name");
	}
	
	edit.OpenScriptModalWindow();

	return false;
}

void ScriptC::Awake()
{
	for (unsigned i = 0; i < scripts.size(); ++i)
	{
		CallLuaFunction(luaScript->lua, i, "Awake");
	}
}

void ScriptC::Initialize()
{
	for (unsigned i = 0; i < scripts.size(); ++i)
	{
		CallLuaFunction(luaScript->lua, i, "Initialize");
	}
}

void ScriptC::Update(float dt)
{
	for (unsigned i = 0; i < scripts.size(); ++i)
	{
		CallLuaFunction(luaScript->lua, i, "Update", dt);
	}
}

void ScriptC::CollisionEnter(Entity& ent)
{
	for (unsigned i = 0; i < scripts.size(); ++i)
	{
		CallLuaFunction(luaScript->lua, i, "CollisionEnter", EntityWrapper(&ent));
	}
}

void ScriptC::CollisionExit(Entity& ent)
{
	for (unsigned i = 0; i < scripts.size(); ++i)
	{
		CallLuaFunction(luaScript->lua, i, "CollisionExit", EntityWrapper(&ent));
	}
}

void ScriptC::CollisionStay(Entity& ent)
{
	for (unsigned i = 0; i < scripts.size(); ++i)
	{
		CallLuaFunction(luaScript->lua, i, "CollisionStay", EntityWrapper(&ent));
	}
}

void ScriptC::SetupScripts()
{
	luaScript = std::make_unique<LuaScript>();
	luaScript->Setup();
	unsigned i = 0;
	// load and execute from file
	for (const auto& item : scripts)
	{
		std::string directory = item;
		try {
			luaScript->lua.script(R"(
				function Awake ()
				end

				function Initialize ()
				end

				function Update (dt)
				end

				function CollisionEnter (ent)
				end

				function CollisionExit (ent)
				end

				function CollisionStay (ent)
				end
				)");
			if(owner)
				(luaScript->lua)["this"] = EntityWrapper(owner);
			luaScript->lua.script_file(directory);
			std::stringstream ss;
			ss << i++ << ":";
			(luaScript->lua)[ss.str() + "Awake"] = (luaScript->lua)["Awake"];
			(luaScript->lua)[ss.str() + "Initialize"] = (luaScript->lua)["Initialize"];
			(luaScript->lua)[ss.str() + "Update"] = (luaScript->lua)["Update"];
			(luaScript->lua)[ss.str() + "CollisionEnter"] = (luaScript->lua)["CollisionEnter"];
			(luaScript->lua)[ss.str() + "CollisionExit"] = (luaScript->lua)["CollisionExit"];
			(luaScript->lua)[ss.str() + "CollisionStay"] = (luaScript->lua)["CollisionStay"];

		}
		catch (sol::error& error)
		{
			CONSOLE_ERROR(error.what(), "(Before awake)");
		}
	}
	ClearLuaVariables(luaScript->lua);
	std::vector<std::string> orderedNames;
	for (const auto& item : luaScript->lua)
	{
		auto itemName = item.first.as<std::string>();
		if (itemName.size() && itemName[0] != '_')
			orderedNames.insert(std::lower_bound(orderedNames.begin(), 
				orderedNames.end(), itemName), itemName);
	}
	for (const auto& item : orderedNames)
	{
		TestVariableTypes<ALLLUATYPES>(luaScript->lua[item], item, *this);
	}
}


void ScriptC::ClearLuaVariables(const sol::state& state)
{
	for (auto& item : variableData)
	{
		if (state[item.first].valid()) continue;
		ClearLuaVariable(*this, item);
	}
}

void ScriptC::EditorSetup()
{
	/*UpdateComponentData(scripts);
	SetupScripts();
	variableData.clear();
	for (const auto& item : luaScript->lua)
	{
		TestVariableTypes<ALLLUATYPES>(item, *this);
	}
	UpdateSerializables();*/
}

void ScriptC::UpdateLuaVariables()
{
	for (const auto& item : variableData)
		UpdateLuaVariable(luaScript->lua, item);
}

void ScriptC::ReflectLuaVariables()
{
	for (const auto& item : luaScript->lua)
	{
		auto n = item.first.as<std::string>();
		auto iter = variableData.find(n);
		if (iter != variableData.end())
		{
			ReflectLuaVariable(item.second, iter->second);
		}
	}
}

void ScriptC::SetVariable(std::string name, sol::object obj)
{
  if (obj.is<EntityWrapper>())
  {
    luaScript->lua[name] = obj.as<EntityWrapper>();
    return;
  }
	TestSetVariable< ALLLUATYPES>(name, obj, luaScript->lua);
}

bool ScriptC::TestCompilation() const
{
	for (const auto& item : scripts)
	{
		auto ss = std::make_unique<LuaScript>();
		ss->Setup();
		std::string directory = item;
		try {
			ss->lua.script(R"(
				function Awake ()
				end

				function Initialize ()
				end

				function Update (dt)
				end

				function CollisionEnter (ent)
				end

				function CollisionExit (ent)
				end

				function CollisionStay (ent)
				end
				)");
			if (owner)
				(ss->lua)["this"] = EntityWrapper(owner);
			ss->lua.script_file(directory);

		}
		catch (sol::error& error)
		{
			return false;
		}
	}
	return true;
}

void ScriptC::CallLuaFunction(std::string funcName)
{
	if (!luaScript->lua[funcName].valid()) 
		CONSOLE_ERROR("Could not find lua function ", funcName, "!");
	else
	{
		luaScript->lua[funcName].call();
	}
}

void ScriptC::ConvertSerializedData(const SerializeElem & e,
	const std::string & objName)
{
	auto strVec = e.FirstChildElement("vector-string");
	if (strVec)
	{
		auto& children = strVec->GetChildren();
		for (const auto& item : children)
		{
			if (item->Name() == "Scripts")
			{
				auto& s_scripts = item->GetChildren();
				for (auto& scriptItem : s_scripts)
				{
					auto& newScript = scripts.emplace_back();
					scriptItem->QueryText(newScript);
				}
			}
		}
	}
	SetupScripts();
	Serializable::ConvertSerializedData(e, objName);
}
