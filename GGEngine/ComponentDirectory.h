#pragma once
#include <tuple>
#include <string>
#include <functional>
#include <map>
#include <memory>
#include "Component.h"
#include "ConstexprStrings.h"
#include "Event.h"

class Component;

struct ComponentMap
{
	std::map<COMPONENTTYPE, std::function<std::unique_ptr<Component>()>>
		componentMap;

	template<typename T>
	void InsertToMap()
	{
		componentMap.emplace(T::GetType(), &std::make_unique<T>);
	}

	template<typename ...Ts>
	void AddToMap()
	{
		using expander = int[];
		(void)expander {
			0, ((void)InsertToMap<Ts>(), 0)...
		};
	}
	auto operator[](COMPONENTTYPE type)
	{
		return componentMap[type];
	}
};


struct EventMap
{
	std::map<EVENTTYPE, std::function<std::unique_ptr<Event>()>>
		eventMap;

	template<typename T>
	void InsertToMap(const T& obj)
	{
		UNREFERENCED_PARAMETER(obj);
		eventMap.emplace(obj.GetType(), &std::make_unique<T>);
	}
	template<typename T>
	void InsertToMap()
	{
		eventMap.emplace(T::GetType(), &std::make_unique<T>);
	}

	template<typename ...Ts>
	void AddToMap()
	{
		using expander = int[];
		(void)expander {
			0, ((void)InsertToMap<Ts>(), 0)...
		};
	}
	auto operator[](EVENTTYPE type)
	{
		return eventMap[type];
	}
};
