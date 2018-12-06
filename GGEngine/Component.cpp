
#include "stdafx.h"

Component::Component() : 
	c_type{ COMPONENTTYPE::NONE },
disabled{ false }, owner{ nullptr }
{
}

Component::~Component()
{
}


std::unique_ptr<Component> Component::Clone()
{
	auto returnptr = COMPONENTMAP[c_type]();
	UpdateComponentData();
	Serializer ss;
	auto& rootElem = ss.NewElement("Root");
	Serialize(rootElem);
	returnptr->AddSerializeData();
	returnptr->ConvertSerializedData(rootElem);
	returnptr->c_type = c_type;
	returnptr->disabled = disabled;
	return std::move(returnptr);
}


void Component::CopySerializableData(const Component& other)
{
	TOOLS::Assert(other.c_type == c_type, "Cannot copy data from a component of a different type!");
	if (other.c_type != c_type) return;
	Serializer ss;
	auto& rootElem = ss.NewElement("Root");
	other.Serialize(rootElem);
	AddSerializeData();
	ConvertSerializedData(rootElem);
	disabled = other.disabled;
}

void Component::SetupLuaCallableFunctions()
{
	for (auto& item : functionList)
	{
		item->ActivateFunction(*owner);
	}
}

void Component::SetType(COMPONENTTYPE type)
{
	c_type = type;
}

void Component::UpdateRequiredComponents(std::vector<std::unique_ptr<Component>>& vec)
{
	for (auto& item : requiredComponents)
	{
		bool found = false;
		for (auto& item2 : vec)
		{
			if (item2->GetType() == item->GetType())
			{
				item->SetPointer(item2.get());
				found = true;
				break;
			}
		}
		TOOLS::Assert(found || !item->required, "Cannot copy data from a component of a different type!");
	}
}
std::unique_ptr<GeneralCallableFunction> Component::GetCallableFunction(const std::string& str)
{
	for (auto& item : functionList)
	{
		if (item->name == str)
		{
			return std::move(item->Clone());
		}
	}
	return nullptr;
}

unsigned Component::GetGameSpaceID() const
{
	return owner->entityList->GetID();
}

void Component::RemoveOptionalComponent(COMPONENTTYPE cc)
{
	for (auto&item : requiredComponents)
	{
		if (item->required && item->GetType() == cc)
		{
			item->RemovePointer();
			return;
		}
	}
}
