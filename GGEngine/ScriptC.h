#pragma once
#include "EntityWrapper.h"
#include "sol.hpp"
#include <set>
#include <unordered_map>
#include <variant>
#include "Vector.h"

class LuaScript;
#define ALLLUATYPES float, bool, Vector3, Vector2, Vector4, std::string, std::vector<std::string>
enum class LUATYPE{FLOAT, BOOL, VEC3, VEC2, VEC4, STRING, STRINGLIST, TYPENUM};

class ScriptC final :
	public Component
{
	friend class ComponentWindow;

	std::vector<unsigned> scriptIDs;
	std::unique_ptr<LuaScript> luaScript;

	// Relative Path Name - Script Name
	using scriptInfo = std::pair<std::string, std::string>;

public:
	std::vector<std::string> scripts;
	std::unordered_map<std::string, 
		std::pair<LUATYPE, std::variant<ALLLUATYPES>>> variableData;
	ScriptC();
	void ClearLuaVariables(const sol::state&);
	void AddSerializeData(LuaScript* luaScript = nullptr) override;
	bool EditorGUI(Editor& edit) override;
	void Awake() override;
	void Initialize() override;
	void Update(float) override;
	void CollisionEnter(Entity&) override;
	void CollisionExit(Entity&) override;
	void CollisionStay(Entity&) override;
	void SetupScripts();
	void EditorSetup() override;
	void UpdateLuaVariables();
	template<typename ... Args>
	void CallLuaFunction(sol::state& luaState, unsigned i, const std::string& str, Args... args)
	{
		std::stringstream ss;
		ss << i << ":";
		//(luaState)["this"] = EntityWrapper(owner);
		//sol::protected_function f = (luaState)[ss.str() + str];
		sol::protected_function f = (luaState)[str];
		auto result = f(args...);
		if (f && !result.valid())
		{
			// An error has occured
			sol::error err = result;
			std::string what = err.what();
			CONSOLE_ERROR(what);
		}
	}
	template<typename T>
	void SetLuaVariable(std::string name, T value)
	{
		luaScript->lua[name] = value;
	}
	template<typename T>
	T GetLuaVariable(std::string name) const
	{
		for (const auto& item : luaScript->lua)
		{
			auto n = item.first.as<std::string>();
			if (n == name)
			{
				return item.second.as<T>();
			}
		}
		std::stringstream ss;
		ss << "Could not find lua variable " << name << "!";
		throw std::runtime_error(ss.str());
	}
	void CallLuaFunction(std::string);
	static constexpr auto GetType() { return COMPONENTTYPE::SCRIPT; }

	void ConvertSerializedData(const SerializeElem& e,
		const std::string& objName = "") override;
	void ReflectLuaVariables();

	sol::object GetVariable(std::string name) {
		for (const auto& item : luaScript->lua)
		{
			auto n = item.first.as<std::string>();
			if (n == name)
			{
				return item.second;
			}
		}
		std::stringstream ss;
		ss << "Could not find lua variable " << name << "!";
		throw std::runtime_error(ss.str());
	}
	void SetVariable(std::string name, sol::object obj);
	bool TestCompilation()const;
};

