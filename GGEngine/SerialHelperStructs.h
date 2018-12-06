#pragma once
#include <memory>
#include <vector>
#include <array>
#include "ConstexprStrings.h"
#include <glm.hpp>
#include "Matrix4x4.h"

//This list contains all types that can currently be serialized
#define ALLSERIALIZEDTYPES float, std::string, \
std::vector<COMPONENTTYPE>, bool, int, Vector2, std::vector<int>, \
std::vector<std::string>, std::vector<Vector2>, Vector4, \
CallableFunctionPtr, Vector3, std::vector<Vector3>, \
std::vector<Vector4>, std::vector<float>, EventList, std::vector<AudioInfo>, unsigned, std::vector<SerialMesh>, Matrix4x4, \
std::vector<sBone>, Node, std::vector<sAnimation>, std::vector<sChannel>, std::vector<Matrix4x4>

#define ALLSERIALIZEDNAMES "float", "string", \
"vector-componenttype", "bool", "int", "vector2", "vector-int", "vector-string", \
"vector-vector2", "vector4", "callableFunction", "vector3", "vector-vector3", \
"vector-vector4", "vector-float", "events", "vector-audioInfo", "unsigned", \
"vector-serialmesh", "matrix4", "vector-sbone", "node", "vector-anim", "vector-channel", "vector-matrix4"

class Event;

using EventList = std::vector<std::unique_ptr<Event>>;
class Entity;
class Vector4;
struct SoundStruct;
struct GeneralCallableFunction;
using CallableFunctionPtr = std::unique_ptr<GeneralCallableFunction>;
struct SerialMesh;
struct sBone;
struct Node;
struct sAnimation;
struct sChannel;

struct StringStruct
{
	std::array<char, 260> container;
	StringStruct() :container({ 0 })
	{
		container[0] = 0;
	}
};

struct SerialDefaults
{
	std::string name;
	std::string tooltip;
	bool display = true;
};

template<typename T>
struct SerialNameContainer: public SerialDefaults
{
	T* item;
	T container;
	SerialNameContainer(const std::string& str, T* it, const std::string& tt) :
		item(it), SerialDefaults{ str, tt }
	{}
};

template<typename T>
struct SerialNameContainer<std::vector<T>> : public SerialDefaults
{
	std::vector<T>* item;
	std::vector<T> container;
	SerialNameContainer(const std::string& str, std::vector<T>* it, const std::string& tt) :
		item(it), SerialDefaults{ str, tt }
	{}
};

template<>
struct SerialNameContainer<std::vector<std::string>> : public SerialDefaults
{
	std::vector<std::string>* item;
	std::vector<StringStruct> container;
	SerialNameContainer(const std::string& str, std::vector<std::string>* it, const std::string& tt) :
		item(it), SerialDefaults{ str, tt }
	{}
};

template<>
struct SerialNameContainer<std::string> : public SerialDefaults
{
	std::string* item;
	StringStruct container;
	SerialNameContainer(const std::string& str, std::string* it, const std::string& tt) :
		item(it), SerialDefaults{ str, tt }
	{}
};

template<>
struct SerialNameContainer<CallableFunctionPtr> : public SerialDefaults
{
	CallableFunctionPtr* item;
	int componentID, functionID;
	std::vector<std::string> componentFunctionNames;
	SerialNameContainer(const std::string& str, CallableFunctionPtr* it, const std::string& tt) :
		item(it), componentID(999), functionID(0), SerialDefaults{ str, tt }
	{}
};

template<typename ... Args>
struct SerialTupleContainer
{
	std::tuple <std::vector<SerialNameContainer<Args>>...> serializedItems;
	static std::array<std::string, 25> serializedNames;
	//static constexpr auto serializedNames = strArr(ALLSERIALIZEDNAMES);
};


template<typename T>
void UpdateSerializableType(SerialNameContainer<T>& item)
{
	item.container = *item.item;
}

void UpdateSerializableType(SerialNameContainer<std::string>& vec);


template<typename T>
void UpdateSerializableType(SerialNameContainer<std::vector<T>>& item)
{
	item.container = *item.item;
}

void UpdateSerializableType(SerialNameContainer<
	std::vector<std::string>>&vec);
void UpdateSerializableType(SerialNameContainer<
	std::vector<SoundStruct>>&vec);
void UpdateSerializableType(SerialNameContainer<EventList>& vec);
void UpdateSerializableType(SerialNameContainer<CallableFunctionPtr>& vec);

template<typename T>
void UpdateData(SerialNameContainer<T>& item)
{
	*item.item = item.container;
}

template<typename T>
void UpdateData(SerialNameContainer<std::vector<T>>& item)
{
	*item.item = item.container;
}
void UpdateData(SerialNameContainer<std::string>& vec);
void UpdateData(SerialNameContainer<std::vector<SoundStruct>>& vec);
void UpdateData(SerialNameContainer<std::vector<std::string>>& vec);
void UpdateData(SerialNameContainer<EventList>& vec);
void UpdateData(SerialNameContainer<CallableFunctionPtr>& vec);

template<typename T>
void UpdateData(std::vector<T>& vec)
{
	for (auto&item : vec)
		UpdateData(item);
}
template<typename T>
void UpdateSerializableType(std::vector<T>& vec)
{
	for (auto&item : vec)
		UpdateSerializableType(item);
}
struct AudioInfo;

void UpdateData(SerialNameContainer<std::vector<AudioInfo>>& item);
void UpdateSerializableType(SerialNameContainer<std::vector<AudioInfo>>& item);