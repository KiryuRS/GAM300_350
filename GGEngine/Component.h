#pragma once
#include <functional>
#include <tuple>
#include <sol.hpp>
#include "LuaScript.h"

// When adding components, remember to add to the 3 areas below!
// ADD IT IN ALPHABETICAL ORDER. THANK YOU
enum class COMPONENTTYPE
{
	NONE,
	IDENTIFIER,
	CAMERA,
	TRANSFORM,
	SCRIPT,
	STATICMESH,
	SKELETALMESH,
	SOUND,
	RIGIDBODY,
	PLANECOLLIDER,
	AABBCOLLIDER,
	OBBCOLLIDER,
	SPHERECOLLIDER,
	CAPSULECOLLIDER,
	ST_MESH_RENDERER,
	SK_MESH_RENDERER,
	CONTROLLER,
	POINTLIGHT,
	PLAYER,
	DIRECTIONALLIGHT,
	SPOTLIGHT,
	PARTICLEEMITTER,
	SPRITE2D,
	BUTTON,
	BUTTONMANAGER,
	COMPONENTNUM
};

#define COMPONENTTYPES \
NoneC, IdentifierC, CameraC, TransformC, ScriptC, StaticMeshC, \
SkeletalMeshC, SoundC, RigidBodyC, ColliderPlane, ColliderAABB, ColliderOBB, ColliderSphere, ColliderCapsule, \
StaticMeshRendererC, SkeletalMeshRendererC, ControllerC, PointLightC, PlayerC, DirectionalLightC, SpotLightC, ParticleEmitterC, \
Sprite2DC, ButtonC, ButtonManagerC

namespace ComponentDirectory
{
	static constexpr auto names = strArr( "Add a Component", "Identifier", "Camera", "Transform", "Script",
										  "StaticMesh", "SkeletalMesh", "Sound", "RigidBody", "PlaneCollider",
										  "AABBCollider", "OBBCollider", "SphereCollider", "CapsuleCollider", "StaticMeshRenderer",
										  "SkeletalMeshRenderer", "Controller", "PointLight", "Player", "DirectionalLight", "SpotLight",
										  "ParticleEmitter", "Sprite2D", "ButtonC", "ButtonManagerC");
}

class EntityList;
class Entity;
struct ComponentObjectBase;
template<typename T>
struct ComponentObject;

struct GeneralCallableFunction : public Serializable
{
	std::string name;
	COMPONENTTYPE type;
	GeneralCallableFunction(const std::string& str) : name(str)
	{}
	virtual void ActivateFunction(Entity&) = 0;
	virtual std::unique_ptr<GeneralCallableFunction> Clone() = 0;
	virtual ~GeneralCallableFunction() {}
};

// Declaring so that we can make use of the index_sequence
template <typename T, typename U, typename ... V>
struct CallableFunctionHolder;

class Component : public Serializable
{
	friend class Entity;
	std::vector<std::unique_ptr<Entity>> newEntities;
public:
	COMPONENTTYPE c_type;
	std::vector<std::unique_ptr<ComponentObjectBase>> requiredComponents;
	std::vector<std::unique_ptr<GeneralCallableFunction>> functionList;
	EntityList* entityList;
	Entity* owner;
	bool disabled;
	std::vector<std::string> callableFunctions;
	Component();
	//Needs the owner to be valid before calling this
	virtual void AddSerializeData(LuaScript* = nullptr){}
	virtual void Awake() {}
	virtual void Initialize() {}
	virtual void Uninitialize() {}
	virtual void Update(float) {}
	virtual void CollisionEnter(Entity&) {}
	virtual void CollisionExit(Entity&) {}
	virtual void CollisionStay(Entity&) {}
	virtual void DestroyUpdate() {}
	//Use imgui functions to create your own ui
	//returned bool tells if serializables need to be check if they are modified
	//in order to put the asterisk
	virtual bool EditorGUI(Editor&) { return false; }
	virtual void EditorUpdate(float) {}
	virtual void EditorSetup() {}
	virtual void Render(){}
	unsigned GetGameSpaceID() const;

	//Helper functions
	void RemoveOptionalComponent(COMPONENTTYPE cc);
	std::unique_ptr<GeneralCallableFunction> GetCallableFunction(const std::string& str);
	std::unique_ptr<Component> Clone();
	void SetType(COMPONENTTYPE);
	void UpdateRequiredComponents(std::vector<std::unique_ptr<Component>>& vec);
	void CopySerializableData(const Component& other);
	void SetupLuaCallableFunctions();
	COMPONENTTYPE GetType() const { return c_type; }
	Component& operator=(const Component&) = delete;
	Component(Component&&) = delete;
	Component(const Component& other) = delete;
	virtual ~Component();

	//Template monsters
	void RequiredComponents() {}
	template<typename T, typename ... Args>
	void RequiredComponents(T*& item, Args*&... args)
	{
		requiredComponents.emplace_back(std::make_unique<ComponentObject<T>>((&item)));
		requiredComponents.back()->required = true;
		RequiredComponents(args...);
	}

	void OptionalComponents() {}
	template<typename T, typename ... Args>
	void OptionalComponents(T*& item, Args*&... args)
	{
		requiredComponents.emplace_back(std::make_unique<ComponentObject<T>>((&item)));
		OptionalComponents(args...);
	}

	template<typename T, typename ... Args>
	T& CreateEntity(Args&&... args)
	{
		newEntities.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
		return static_cast<T&>(*(newEntities.back()));
	}

	template<typename T, typename ... args>
	void AddCallableFunction(const std::string& str, void (T::*fn)(args...),
		const std::vector<std::string>& paramNames, LuaScript* script)
	{
		if (script)
		{
			auto item = std::find_if(functionList.begin(), functionList.end(),
				[&](CallableFunctionPtr& funcPtr)
			{
				if (funcPtr->name == str)
					return true;
				return false;
			});
			auto func =
				[fn](ComponentWrapper* cWrapper, args... Args)->void {
				auto newFunc = std::mem_fn(fn);
				newFunc(static_cast<T*>(cWrapper->comp), Args...);
			};
			script->componentFunctions->set(str, func);
			if (item != functionList.end()) return;
		}
		callableFunctions.emplace_back(GetFunctionName(fn, str));
		using Indices = std::make_index_sequence<sizeof...(args)>;
		functionList.emplace_back(std::make_unique<CallableFunctionHolder<T, Indices, args...>>(str, fn, paramNames));
	}


	template<typename T, typename U, typename ... Args>
	void AddCallableFunction(const std::string& str, U(T::*fn)(Args...),
		LuaScript* script)
	{
		if (script)
		{
			auto item = std::find_if(functionList.begin(), functionList.end(),
				[&](CallableFunctionPtr& funcPtr)
			{
				if (funcPtr->name == str)
					return true;
				return false;
			});
			auto func =
				[fn](ComponentWrapper* cWrapper, Args... aa) {
				auto newFunc = std::mem_fn(fn);
				return newFunc(static_cast<T*>(cWrapper->comp), aa...);
			};
			script->componentFunctions->set(str, func);
		}
		callableFunctions.emplace_back(GetFunctionName(fn, str));
	}
	template<typename T, typename U, typename ... Args>
	void AddCallableFunction(const std::string& str, U(T::*fn )(Args...)const,
		LuaScript* script)
	{
		if (script)
		{
			auto item = std::find_if(functionList.begin(), functionList.end(),
				[&](CallableFunctionPtr& funcPtr)
			{
				if (funcPtr->name == str)
					return true;
				return false;
			});
			auto func =
				[fn](ComponentWrapper* cWrapper, Args... aa) {
				auto newFunc = std::mem_fn(fn);
				return newFunc(static_cast<T*>(cWrapper->comp), aa...);
			};
			script->componentFunctions->set(str, func);
		}
		callableFunctions.emplace_back(GetFunctionName(fn, str));
	}

	template<typename T>
	SerialNameContainer<T>& AddSerializable(const std::string& title, T& value, 
		 const std::string& tooltip, LuaScript* script)
	{
		if (script)
		{
			auto getterFunc = [title](ComponentWrapper* cWrapper)
			{
				return cWrapper->comp->GetData<T>(title);
			};
			auto setterFunc = [title](ComponentWrapper* cWrapper, T value)
			{
				return cWrapper->comp->SetData(title, value);
			};
			script->componentFunctions->set(title, sol::property(getterFunc, setterFunc));
		}
		return Serializable::AddSerializable(title, value, tooltip);
	}

	template<>
	SerialNameContainer<EventList>& AddSerializable(const std::string& title, EventList& value,
		const std::string& tooltip, LuaScript*)
	{
		return Serializable::AddSerializable(title, value, tooltip);
	}
	virtual void ConvertSerializedData(const SerializeElem& e, 
		const std::string& objName = "")
	{
		Serializable::ConvertSerializedData(e, objName);
	}
};


class NoneC : public Component
{
public:
	NoneC() {}
	static constexpr COMPONENTTYPE GetType()
	{
		return COMPONENTTYPE::NONE;
	}
};




struct ComponentObjectBase
{
	bool required = false;
	virtual void SetPointer(Component*) const = 0;
	virtual void RemovePointer() = 0;
	virtual COMPONENTTYPE GetType() const = 0;
	virtual ~ComponentObjectBase() {}
};

template<typename T>
struct ComponentObject : public ComponentObjectBase
{
	T** pointer;
	ComponentObject(T** c) :
		pointer(c)
	{ }

	void SetPointer(Component* comp) const override
	{
		if (!comp)
			throw("Required component not found!!!");
		*pointer = static_cast<T*>(comp);
	}

	void RemovePointer() override
	{
		*pointer = nullptr;
	}

	COMPONENTTYPE GetType() const override
	{
		return T::GetType();
	}
};

template<typename Type, size_t ... Is, typename ... Args>
struct CallableFunctionHolder<Type, std::index_sequence<Is...>, Args...> : public GeneralCallableFunction
{
	std::tuple<Args...> items;
	// Function Pointer calling from Type class and taking in vardiac arguments returning void
	void (Type::*calledFunction)(Args...);
	const std::vector<std::string> paramNames;

	template<typename T>
	CallableFunctionHolder(const std::string& str, T fn,
		const std::vector<std::string>& names) :
		GeneralCallableFunction(str), calledFunction(fn),
		paramNames(names)
	{
		//if constexpr(sizeof...(Args) >= 1)
			AddVariadicSerializable<Args...>();
		type = Type::GetType();
	}
	template<typename ... args2, 
		typename std::enable_if<sizeof...(args2) == 0, int> ::type = 0>
	void AddVariadicSerializable()
	{}

	template<typename T, typename ... args2>
	void AddVariadicSerializable()
	{
		constexpr unsigned sizeDiff = sizeof...(Args) - sizeof...(args2);
		std::stringstream ss;
		if (paramNames.size() >= sizeDiff)
			ss << paramNames[sizeDiff - 1];
		else
			ss << "Item" << sizeDiff;
		AddSerializable(ss.str(), std::get<sizeDiff - 1>(items), "");
		//if constexpr(sizeof...(args2) >= 1)
			AddVariadicSerializable<args2...>();
	}

	void ActivateFunction(Entity& ent) override
	{
		auto comp = ent.GetComponent(Type::GetType());
		auto newFunc = std::mem_fn(calledFunction);
		if (comp)
			newFunc(static_cast<Type*>(comp), std::get<Is>(items)...);
		TOOLS::Assert(comp, "Could not find function!");
	}

	std::unique_ptr<GeneralCallableFunction> Clone() override
	{
		using Indices = std::make_index_sequence<sizeof...(Args)>;
		auto returnPtr =
			std::make_unique<CallableFunctionHolder<Type, Indices, Args...>>(name, calledFunction, paramNames);
		returnPtr->items = items;
		returnPtr->UpdateSerializables();
		return std::move(returnPtr);
	}
};
