#pragma once
#include "Entity.h"
#include <type_traits>
#include "Serializable.h"
#include "MetaBoilerplates.h"
class ComponentWrapper;
class EntityWrapper;


template<typename T>
constexpr std::string ConvertHelper()
{
	if constexpr (std::is_same<T, void>())
		return "void";
	if constexpr (std::is_same<T, EntityWrapper>())
		return "Entity";
	if constexpr (std::is_same<T, ComponentWrapper>())
		return "Component";
	if constexpr (std::is_same < T, sol::table& >())
		return "SolTable";
	if constexpr (std::is_same < T, sol::object >())
		return "Variant";
	return "UnknownType";
}
template<typename T>
constexpr std::string ConvertHelper2()
{
	constexpr auto index = MySTL::typeIndex<T, ALLSERIALIZEDTYPES>::value;
	return SerialTupleContainer<ALLSERIALIZEDTYPES>::serializedNames[index];

}

template<typename T>
constexpr std::string ConvertTypeToName()
{
	if constexpr (!MySTL::ContainType<T, ALLSERIALIZEDTYPES>::value)
		return ConvertHelper<T>();
	else
		return ConvertHelper2<T>();
}

template<typename T, typename ...Args>
void AddParamName(std::stringstream& ss)
{
	if constexpr (std::is_same<T, void>()) return;
	ss << ConvertTypeToName<T>();
	if constexpr (sizeof...(Args) != 0)
	{
		ss << ", ";
		AddParamName<Args...>(ss);
	}
}
template<typename T, typename U, typename ... Args>
std::string GetFunctionName(U(T::*)(Args...), const std::string& name)
{
	std::stringstream ss;
	ss << ConvertTypeToName<U>() << " " << name << "(";
	if constexpr (sizeof...(Args) != 0)
		AddParamName<Args...>(ss);
	ss << ")";
	return ss.str();
}
template<typename T, typename U, typename ... Args>
std::string GetFunctionName(U(T::*)(Args...) const, const std::string& name)
{
	std::stringstream ss;
	ss << ConvertTypeToName<U>() << " " << name << "(";
	if constexpr (sizeof...(Args) != 0)
		AddParamName<Args...>(ss);
	ss << ")";
	return ss.str();
}

//Helper functions to help in setting up lua
class LuaScript {
public:
	void Setup();
	std::vector<std::string> luaFunctionNames;
	sol::state lua;
	std::unique_ptr<sol::simple_usertype<ComponentWrapper>> componentFunctions;

	LuaScript()
	{}

private:
	template<typename T, typename U, typename ... Args>
	void AddSerializedFunction(sol::simple_usertype<EntityWrapper>& wrap, bool addToVector,
		const std::string& funcName, T(U::*funcPtr)(Args...))
	{
		if (addToVector)
		{
			luaFunctionNames.emplace_back(GetFunctionName( funcPtr, funcName));
		}
		wrap.set(funcName, funcPtr);
	}
	template<typename T, typename U, typename V, typename ... Args>
	void AddSerializedFunction(sol::simple_usertype<V>& wrap, bool addToVector,
		const std::string& funcName, T(U::*funcPtr)(Args...))
	{
		if (addToVector)
		{

			luaFunctionNames.emplace_back(GetFunctionName(funcPtr, funcName));
		}
		wrap.set(funcName, funcPtr);
	}
	template<typename T, typename ...Args>
	void AddAllComponents(Entity& ent)
	{
		ent.AddComponent(T::GetType(), false);
		if constexpr(sizeof...(Args) > 0)
			AddAllComponents<Args...>(ent);
	}

};