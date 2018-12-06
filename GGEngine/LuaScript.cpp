#include "stdafx.h"
#include "LuaScript.h"
#include "SkeletalMeshRendererC.h"
#include "StaticMeshRendererC.h"

float VectorDist(const Vector3& lhs, const Vector3& rhs)
{
	return (lhs - rhs).Length();
}
template<typename T>
float VectorDot(const T& lhs, const T& rhs)
{
	return lhs * rhs;
}

template <typename T>
void PrintOverload(const T& printout)
{
	CONSOLE_SCRIPT(printout);
}

void LuaScript::Setup()
{
	Entity temp;
	lua = {};
	luaFunctionNames.clear();
	AddAllComponents<COMPONENTTYPES>(temp);

	//setup boilerplate types
	lua.open_libraries(sol::lib::base);

	// Constructing for Vector2
	lua.new_usertype<Vector2>("Vector2",
		sol::constructors<Vector2(), Vector2(float, float)>(),
		"x", sol::property(&Vector2::GetX, &Vector2::SetX),
		"y", sol::property(&Vector2::GetY, &Vector2::SetY),
		sol::meta_function::addition, [](const Vector2& l, const Vector2& r) {return l + r; },
		sol::meta_function::subtraction, [](const Vector2& l, const Vector2& r) {return l - r; },
		sol::meta_function::multiplication, sol::overload([](const Vector2& l, const Vector2& r) {return l * r; },
			[](const Vector2& l, float r) {return l * r; })
		);

	// Constructing for Vector3
	lua.new_usertype<Vector3>("Vector3",
		sol::constructors<Vector3(), Vector3(float, float, float)>(),
		"x", sol::property(&Vector3::GetX, &Vector3::SetX),
		"y", sol::property(&Vector3::GetY, &Vector3::SetY),
		"z", sol::property(&Vector3::GetZ, &Vector3::SetZ),
		sol::meta_function::addition, [](const Vector3& l, const Vector3& r){return l + r;},
		sol::meta_function::subtraction, [](const Vector3& l, const Vector3& r) {return l - r; },
		sol::meta_function::multiplication, sol::overload([](const Vector3& l, const Vector3& r) {return l * r; },
			[](const Vector3& l, float r) {return l * r; })
		);

	// Constructing for Vector4
	lua.new_usertype<Vector4>("Vector4",
		sol::constructors<Vector4(), Vector4(float, float, float, float)>(),
		"x", sol::property(&Vector4::GetX, &Vector4::SetX),
		"y", sol::property(&Vector4::GetY, &Vector4::SetY),
		"z", sol::property(&Vector4::GetZ, &Vector4::SetZ),
		"w", sol::property(&Vector4::GetW, &Vector4::SetW),
		sol::meta_function::addition, [](const Vector4& l, const Vector4& r) {return l + r; },
		sol::meta_function::subtraction, [](const Vector4& l, const Vector4& r) {return l - r; },
		sol::meta_function::multiplication, sol::overload([](const Vector4& l, const Vector4& r) {return l * r; },
			[](const Vector4& l, float r) {return l * r; })
		);

	lua["Vector3"]["Unit"] = &Vector3::Unit;
	lua["Vector3"]["Normalize"] = &Vector3::Normalize;
	lua["Vector3"]["Length"] = &Vector3::Length;lua.set_function("VectorDot2", VectorDot<Vector2>);

	lua["Vector4"]["Normalize"] = &Vector4::Normalize;


	// Constructing a vector of std::string
	//lua.new_usertype<std::vector<std::string>>("StringList",
	//	sol::constructors<std::vector<std::string>>());
	////void (StringList::*stringPOP)() = &StringList::pop_back;

	//lua["StringList"]["PushBack"] = &StringList::PushBack;
	//lua["StringList"]["PopBack"] = stringPOP;
	//lua["StringList"]["GetElem"] = &StringList::GetElem;
	//lua["StringList"]["SetElem"] = &StringList::SetElem;
	//lua["StringList"]["Size"] = &StringList::size;
	//lua["StringList"]["Clear"] = &StringList::clear;
	//lua["StringList"]["SetSize"] = &StringList::SetSize;
	//lua["StringList"]["RotateLeft"] = &StringList::RotateLeft;
	//lua["StringList"]["RotateRight"] = &StringList::RotateRight;
	//lua["StringList"]["Erase"] = &StringList::Erase;

	lua.new_usertype<std::vector<std::string>>("StringList",
		sol::constructors<std::vector<std::string>>());
	void (std::vector<std::string>::*stringPB)(const std::string&) = &std::vector<std::string>::push_back;
	void (std::vector<std::string>::*stringPOP)() = &std::vector<std::string>::pop_back;
	std::string& (std::vector<std::string>::*stringSUB)(size_t) = &std::vector<std::string>::operator[];

	lua["StringList"]["PushBack"] = stringPB;
	lua["StringList"]["PopBack"] = stringPOP;
	lua["StringList"]["GetElem"] = [](std::vector<std::string>&vv, size_t pos) -> std::string {
		if (vv.size() <= pos)
			throw std::exception("Trying to get an out of bounds string list!");
		return vv[pos];
	};
	lua["StringList"]["SetElem"] = [](std::vector<std::string>&vv, size_t pos, const std::string& str) {
		if (vv.size() <= pos)
			throw std::exception("Trying to set an out of bounds string list!");
		vv[pos] = str;
	};
	lua["StringList"]["Size"] = &std::vector<std::string>::size;
	lua["StringList"]["RotateLeft"] = [](std::vector<std::string>& vv) {
		// Simple rotation to the left
		std::rotate(vv.begin(), vv.begin() + 1, vv.end());
	};
	lua["StringList"]["RotateRight"] = [](std::vector<std::string>& vv) {
		// Simple rotation to the right
		std::rotate(vv.rbegin(), vv.rbegin() + 1, vv.rend());
	};
	lua["StringList"]["SetSize"] = [](std::vector<std::string>& vv, size_t size) {
		vv.resize(size);
	};

	// Constructing a vector of int
	lua.new_usertype<std::vector<int>>("IntList", sol::constructors<std::vector<int>>());
	// Function pointer helpers
	void (std::vector<int>::*intPB)(const int&) = &std::vector<int>::push_back;
	void (std::vector<int>::*intPOP)() = &std::vector<int>::pop_back;

	lua["IntList"]["PushBack"] = intPB;
	lua["IntList"]["PopBack"] = intPOP;
	lua["IntList"]["GetElem"] = [](std::vector<int>& vv, size_t pos) -> int {
		if (vv.size() <= pos)
			throw std::exception("Trying to get an out of bounds int list!");
		return vv[pos];
	};
	lua["IntList"]["SetElem"] = [](std::vector<int>& vv, size_t pos, int value) {
		if (vv.size() <= pos)
			throw std::exception("Trying to set an out of bounds int list!");
		vv[pos] = value;
	};
	lua["IntList"]["Size"] = &std::vector<int>::size;
	lua["IntList"]["RotateLeft"] = [](std::vector<int>& vv) {
		// Simple rotation to the left
		std::rotate(vv.begin(), vv.begin() + 1, vv.end());
	};
	lua["IntList"]["RotateRight"] = [](std::vector<int>& vv) {
		// Simple rotation to the right
		std::rotate(vv.rbegin(), vv.rbegin() + 1, vv.rend());
	};
	lua["IntList"]["SetSize"] = [](std::vector<int>& vv, size_t size) {
		vv.resize(size);
	};
	
	lua.set_function("VectorDot2", VectorDot<Vector2>);
	lua.set_function("VectorDot3", VectorDot<Vector3>);
	lua.set_function("VectorDot4", VectorDot<Vector4>);
	lua.set_function("RayCast", [](const Vector3 & pos, const Vector3 & dir, float maxDist = FLT_MAX)
	{
		Entity* ent = PHYSICS_S.PhysicsRayCastModel(pos, dir, maxDist);
		if(ent)
			return EntityWrapper(ent);
		return EntityWrapper();
	});
	lua.set_function("RayCastPoint", [](const Vector3 & pos, const Vector3 & dir, float maxDist = FLT_MAX)
	{
		Vector3 Contact;
		Entity* ent = (PHYSICS_S.PhysicsRayCastContactData(pos, dir, maxDist, Contact));
		//CONSOLE_LN(Contact);
		return Contact;
	});
	lua.set_function("PreloadPrefab", [](const std::string& str)
	{
		std::string newStr = PREFAB_DIRECTORY + str + ".pfb";
		Entity ent;
		auto loaded = ent.LoadPrefab(newStr);
		if (loaded)
		{
			auto smesh = ent.GetComponent<StaticMeshC>();
			if (smesh)
				GFX_S.GetStaticMesh(smesh->base.modelDirectory);
			auto skmesh = ent.GetComponent<SkeletalMeshC>();
			if (skmesh)
				GFX_S.GetSkeletalMesh(skmesh->base.modelDirectory);
			return true;
		}
		else
		{
			std::stringstream ss;
			ss << "Could not find prefab " << str << "!";
			throw std::runtime_error(ss.str());
		}
		return loaded;
	});

	//setup entity info
	auto entityFunctions = lua.create_simple_usertype<EntityWrapper>("Entity",
		sol::constructors<EntityWrapper()>());
	bool addToVector = false;

	if (!luaFunctionNames.size()) addToVector = true;

	AddSerializedFunction(entityFunctions, addToVector,
		"GetComponent", &EntityWrapper::GetComponent);
	AddSerializedFunction(entityFunctions, addToVector,
		"CreateEntity", &EntityWrapper::CreateEntity);
	AddSerializedFunction(entityFunctions, addToVector,
		"AddComponent", &EntityWrapper::AddComponent);
	AddSerializedFunction(entityFunctions, addToVector,
		"FindEntity", &EntityWrapper::FindEntity);
  AddSerializedFunction(entityFunctions, addToVector,
    "EntityExists", &EntityWrapper::EntityExists);
	AddSerializedFunction(entityFunctions, addToVector,
		"Destroy", &EntityWrapper::Destroy);
	AddSerializedFunction(entityFunctions, addToVector,
		"LoadPrefab", &EntityWrapper::LoadPrefab);
	AddSerializedFunction(entityFunctions, addToVector,
		"GetChild", &EntityWrapper::GetChild);
	AddSerializedFunction(entityFunctions, addToVector,
		"GetParent", &EntityWrapper::GetParent);
	AddSerializedFunction(entityFunctions, addToVector,
		"GetChildrenCount", &EntityWrapper::GetChildrenCount);
	AddSerializedFunction(entityFunctions, addToVector,
		"ClearChildren", &EntityWrapper::ClearChildren);
	AddSerializedFunction(entityFunctions, addToVector,
		"AddChild", &EntityWrapper::AddChild);
	AddSerializedFunction(entityFunctions, addToVector,
		"GetEntity", &EntityWrapper::GetEntity);
	AddSerializedFunction(entityFunctions, addToVector,
		"Disable", &EntityWrapper::Disable);
	AddSerializedFunction(entityFunctions, addToVector,
		"Enable", &EntityWrapper::Enable);
	AddSerializedFunction(entityFunctions, addToVector,
		"IsDisabled", &EntityWrapper::IsDisabled);
	AddSerializedFunction(entityFunctions, addToVector,
		"FindExternalEntity", &EntityWrapper::FindExternalEntity);
	AddSerializedFunction(entityFunctions, addToVector,
		"DoesExternalEntityExist", &EntityWrapper::DoesExternalEntityExist);
	AddSerializedFunction(entityFunctions, addToVector,
		"CheckNull", &EntityWrapper::CheckNullBool);
	AddSerializedFunction(entityFunctions, addToVector,
		"SetSkybox", &EntityWrapper::SetSkybox);
	AddSerializedFunction(entityFunctions, addToVector,
		"EnableFog", &EntityWrapper::SetFog);
	AddSerializedFunction(entityFunctions, addToVector,
		"SetFogColor", &EntityWrapper::SetFogColor);
	AddSerializedFunction(entityFunctions, addToVector,
		"SearchPrefabName", &EntityWrapper::SearchPrefabName);

	lua.set_function("Print", sol::overload(
		&PrintOverload<std::string>,
		&PrintOverload<Vector2>,
		&PrintOverload<Vector3>,
		&PrintOverload<Vector4>,
		&PrintOverload<int>,
		&PrintOverload<float>,
		&PrintOverload<double>
	));
	lua.set_function("RadToDeg", [](float a) -> float { return CONVERT_RAD_TO_DEG(a); });
	lua.set_function("DegToRad", [](float a) -> float { return CONVERT_DEG_TO_RAD(a); });
	lua.set_function("RandomInt", TOOLS::RandInt);
	lua.set_function("RandomFloat", TOOLS::RandFloat); 
	lua.set_function("VectorDist", VectorDist);
	lua.set_function("Cosine", MATH::cosf_d);
	lua.set_function("ACosine", acosf);
	lua.set_function("Sine", MATH::sinf_d);
	lua.set_function("Floor", MATH::ffloor);
	lua.set_function("Ceil", MATH::fceil);
	lua.set_function("ExtractString", [](const std::string& str, const std::string& finder)
	{
		std::string retStr{};
		// Getting the substr of the str of whatever that is remaining upon finding finder
		size_t pos = str.find(finder);
		if (pos == std::string::npos)
			return retStr;
		// Otherwise we substr the remaining
		pos += finder.length();
		retStr = str.substr(pos);
		return retStr;
	});
	lua.set_function("StringLen", [](const std::string& str)
	{
		return str.length();
	});
	lua.set_function("ConvertToLower", [](const std::string& str)
	{
		std::string retStr = str;
		std::transform(str.begin(), str.end(), retStr.begin(), TOOLS::modifiedtolower);
		return retStr;
	});
	lua.set_function("PhysicsRayCast", [](const Vector3 & pos, const Vector3 & dir, float maxDist, Vector3 & pointOfContact)->EntityWrapper
	{
		return EntityWrapper(PHYSICS_S.PhysicsRayCastContactData(pos, dir, maxDist, pointOfContact));
	});
	lua.set_function("LerpFloat", MATH::Lerp<float>);
	lua.set_function("LerpVector2", MATH::Lerp<Vector2>);
	lua.set_function("LerpVector3", MATH::Lerp<Vector3>);
	lua.set_function("LerpVector4", MATH::Lerp<Vector4>);
	lua.set_function("LookAt", LookAt);
	lua.set_function("Rotate", Rotate);
	lua.set_function("QuaternionToEuler", EulerQuaternion);
	lua.set_function("LerpFloat", MATH::Lerp<float>);
	lua.set_function("LerpVector2", MATH::Lerp<Vector2>);
	lua.set_function("LerpVector3", MATH::Lerp<Vector3>);
	lua.set_function("LerpVector4", MATH::Lerp<Vector4>);
	lua.set_function("PrefabExists", [](const std::string& str)
	{
		std::string newStr = PREFAB_DIRECTORY + str + ".pfb";
		Serializer ss;
		auto result = ss.LoadFile(newStr);
		if (result == SERIALRESULT::SUCCESS) return true;
		return false;
	});
	lua.set_function("DisplayMouse", [](bool toggle) -> void { INPUT_S.ShowMouse(toggle); });
	lua.set_function("MouseLock", [](bool toggle) -> void { INPUT_S.LockMouseIntoCenter(toggle); });
	entityFunctions.set("name", sol::property(&EntityWrapper::GetName, &EntityWrapper::SetName));
	lua.set_usertype("Entity", entityFunctions);
	lua.set_function("GetAnimationDuration", 
		[](const std::string& modelName, int animNum)->double
	{
		auto sk = GFX_S.GetSkeletalMesh(modelName);
		return sk->animations[animNum].duration;
	});
	lua.set_function("ObjectsLoaded",
		[]()->bool
	{
#ifndef EDITOR
		return GFX_S.ObjectsLoaded();
#else
		return true;
#endif

	});

	
	//setup component info
	componentFunctions = 
		std::make_unique<sol::simple_usertype<ComponentWrapper>>
		(lua.create_simple_usertype<ComponentWrapper>("Component"));
	//add functions for all components
	for (auto& comp : temp.components)
		comp->AddSerializeData(this);
	lua.set_usertype("Component", *componentFunctions);
	lua["Component"]["Exists"] = &ComponentWrapper::Exists;
}
