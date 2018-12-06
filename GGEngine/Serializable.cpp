#include "stdafx.h"				// must be the first thing to include
#include "AudioSystem.h"

#pragma warning (disable: 28020)			// Internal C++ disables
#pragma warning (disable : 4312)

Serializable::Serializable()
	: minimized(false)
{}

template<typename ...Ts>
void ExpandUpdateSerializables(std::tuple<Ts...>& tuple)
{
	using expander = int[];
	(void)expander {
		0, ((void)UpdateSerializableType(std::get<Ts>(tuple)), 0)...
	};
}
void Serializable::UpdateSerializables()
{
	ExpandUpdateSerializables(serialTupleContainer.serializedItems);
}

template<typename ...Ts>
void ExpandUpdateData(std::tuple<Ts...>& tuple)
{
	using expander = int[];
	(void)expander {
		0, ((void)UpdateData(std::get<Ts>(tuple)), 0)...
	};
}

void Serializable::UpdateComponentData()
{
	ExpandUpdateData(serialTupleContainer.serializedItems);
}

template<typename T>
void ApplySerializeType(const T& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetText(item);
}

void ApplySerializeType(const COMPONENTTYPE& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetText(ComponentDirectory::names[static_cast<int>(item) + 1].data());
}

void ApplySerializeType(const StringStruct& item,
	SerializeElem& elemPtr)
{
	std::string ss = item.container.data();
	if (ss.size())
		elemPtr.SetText(item.container.data());
}

void ApplySerializeType(const Vector2& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetAttribute("x", item.x);
	elemPtr.SetAttribute("y", item.y);
}

void ApplySerializeType(const Vector3& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetAttribute("x", item.x);
	elemPtr.SetAttribute("y", item.y);
	elemPtr.SetAttribute("z", item.z);
}
void ApplySerializeType(const Vector4& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetAttribute("x", item.x);
	elemPtr.SetAttribute("y", item.y);
	elemPtr.SetAttribute("z", item.z);
	elemPtr.SetAttribute("w", item.w);
}

void ApplySerializeType(const Matrix4x4& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetAttribute("a", item[0]);
	elemPtr.SetAttribute("b", item[1]);
	elemPtr.SetAttribute("c", item[2]);
	elemPtr.SetAttribute("d", item[3]);
	elemPtr.SetAttribute("e", item[4]);
	elemPtr.SetAttribute("f", item[5]);
	elemPtr.SetAttribute("g", item[6]);
	elemPtr.SetAttribute("h", item[7]);
	elemPtr.SetAttribute("i", item[8]);
	elemPtr.SetAttribute("j", item[9]);
	elemPtr.SetAttribute("k", item[10]);
	elemPtr.SetAttribute("l", item[11]);
	elemPtr.SetAttribute("m", item[12]);
	elemPtr.SetAttribute("n", item[13]);
	elemPtr.SetAttribute("o", item[14]);
	elemPtr.SetAttribute("p", item[15]);
}
void ApplySerializeType(const AudioInfo& item,
	SerializeElem& elemPtr)
{
	item.Serialize(elemPtr);
}

void ApplySerializeType(const SerialMesh& item,
	SerializeElem& elemPtr)
{
	item.Serialize(elemPtr);
}
void ApplySerializeType(const sBone& item,
	SerializeElem& elemPtr)
{
	item.Serialize(elemPtr);
}
void ApplySerializeType(const sAnimation& item,
	SerializeElem& elemPtr)
{
	item.Serialize(elemPtr);
}
void ApplySerializeType(const sChannel& item,
	SerializeElem& elemPtr)
{
	item.Serialize(elemPtr);
}


template<typename T>
void ApplySerializeType(const std::vector<T>& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetAttribute("size", (unsigned)item.size());
	for (unsigned count = 0; count < static_cast<unsigned>(item.size()); ++count)
	{
		std::stringstream itemName;
		itemName << "Item" << count;
		auto& elem3 = elemPtr.InsertChild(itemName.str().c_str());
		ApplySerializeType(item[count], elem3);
	}
}

void ApplySerializeType(const SerialNameContainer<CallableFunctionPtr>& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetAttribute("componentType", ComponentDirectory::names[item.componentID + 1].data());
	if (!item.item->get()) return;
	auto& elem3 = elemPtr.InsertChild(item.componentFunctionNames[item.functionID].c_str());
	(*item.item)->Serialize(elem3);
}


void ApplySerializeType(const EventList& item,
	SerializeElem& elemPtr)
{
	elemPtr.SetAttribute("size", (unsigned)item.size());
	for (unsigned count = 0; count < static_cast<unsigned>(item.size()); ++count)
	{
		std::stringstream itemName;
		itemName << EventDirectory::names[static_cast<int>(item[count]->GetType())].data();
		auto& elem3 = elemPtr.InsertChild(itemName.str().c_str());
		item[count]->Serialize(elem3);
	}
}

template<typename T>
void GeneralSerialize(const std::vector<SerialNameContainer<T>>& vec,
	SerializeElem& elemPtr, const std::string& elemName)
{
	if (vec.empty()) return;
	auto& pElement = elemPtr.InsertChild(elemName.c_str());
	for (auto&item : vec)
	{
		auto& elem = pElement.InsertChild(item.name);
		ApplySerializeType(item.container, elem);
	}
}

void GeneralSerialize(const std::vector<SerialNameContainer<CallableFunctionPtr>>& vec,
	SerializeElem& elemPtr, const std::string& elemName)
{
	if (vec.empty()) return;
	auto& pElement = elemPtr.InsertChild(elemName.c_str());
	for (auto&item : vec)
	{
		auto& elem = pElement.InsertChild(item.name.c_str());
		ApplySerializeType(item, elem);
	}
}

template<unsigned I1>
struct SerialExpanderLeaf
{
	template<typename T, typename U>
	SerialExpanderLeaf(SerializeElem& elem,
		const T& tuple, const U& names)
	{
		GeneralSerialize(std::get<I1>(tuple), elem, names[I1].data());
	}
};

template<typename T>
struct SerialExpander
{};

template<unsigned... Indices>
struct SerialExpander<std::index_sequence<Indices...>> :
	SerialExpanderLeaf<Indices>...
{
	template<typename T, typename U>
	SerialExpander(SerializeElem & elem,
		const T& tuple, const U& names) :
		SerialExpanderLeaf<Indices>(elem, tuple, names)...
	{

	}
};

template<typename U, typename ...Ts>
void ExpandSerialize(SerializeElem& elem,
	const std::tuple<Ts...>& tuple, const U& names)
{
	using sequence = std::make_index_sequence<sizeof...(Ts)>;
	SerialExpander<sequence> seq(elem, tuple, names);
}

void Serializable::Serialize(SerializeElem& elem) const
{
	ExpandSerialize(elem, serialTupleContainer.serializedItems,
		serialTupleContainer.serializedNames);
}

template<typename T>
void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<T>& item)
{
	childStr.QueryText(*item.item);
}

void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<Vector2>& item)
{
	childStr.QueryAttribute("x", item.item->x);
	childStr.QueryAttribute("y", item.item->y);
}

void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<Vector3>& item)
{
	childStr.QueryAttribute("x", item.item->x);
	childStr.QueryAttribute("y", item.item->y);
	childStr.QueryAttribute("z", item.item->z);
}

void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<Vector4>& item)
{
	childStr.QueryAttribute("x", item.item->x);
	childStr.QueryAttribute("y", item.item->y);
	childStr.QueryAttribute("z", item.item->z);
	childStr.QueryAttribute("w", item.item->w);
}

void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<Matrix4x4>& item)
{
	childStr.QueryAttribute("a", (*item.item)[0]);
	childStr.QueryAttribute("b", (*item.item)[1]);
	childStr.QueryAttribute("c", (*item.item)[2]);
	childStr.QueryAttribute("d", (*item.item)[3]);
	childStr.QueryAttribute("e", (*item.item)[4]);
	childStr.QueryAttribute("f", (*item.item)[5]);
	childStr.QueryAttribute("g", (*item.item)[6]);
	childStr.QueryAttribute("h", (*item.item)[7]);
	childStr.QueryAttribute("i", (*item.item)[8]);
	childStr.QueryAttribute("j", (*item.item)[9]);
	childStr.QueryAttribute("k", (*item.item)[10]);
	childStr.QueryAttribute("l", (*item.item)[11]);
	childStr.QueryAttribute("m", (*item.item)[12]);
	childStr.QueryAttribute("n", (*item.item)[13]);
	childStr.QueryAttribute("o", (*item.item)[14]);
	childStr.QueryAttribute("p", (*item.item)[15]);
}

void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<CallableFunctionPtr>& item)
{
	std::string attrib;
	childStr.QueryAttribute("componentType", attrib);
	for (unsigned i = 1; i <= ComponentDirectory::names.size(); ++i)
	{
		if (attrib == ComponentDirectory::names[i])
		{
			auto tempComp = COMPONENTMAP[static_cast<COMPONENTTYPE>(i)]();
			tempComp->AddSerializeData();
			--i;
			item.componentFunctionNames.clear();
			auto childElem = childStr.FirstChildElement();
			if (childElem)
			{
				std::string name = childElem->Name();
				for (unsigned j = 0; j < tempComp->functionList.size(); ++j)
				{
					item.componentFunctionNames.emplace_back(tempComp->functionList[j]->name);
					if (name == tempComp->functionList[j]->name)
					{
						item.functionID = j;
						*item.item = tempComp->functionList[j]->Clone();
					}
				}
				if (item.item->get())
					(*item.item)->ConvertSerializedData(*childElem);
				TOOLS::Assert(item.item->get(), "Could not find function ", name, "!");
			}
			item.componentID = i;
			break;
		}
	}
}

void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<COMPONENTTYPE>& item)
{
	std::string componentType;
	childStr.QueryText(componentType);
	for (unsigned i = 1; i < ComponentDirectory::names.size(); ++i)
	{
		if (ComponentDirectory::names[i].data() == componentType)
		{
			*item.item = static_cast<COMPONENTTYPE>(i - 1);
			return;
		}
	}
	TOOLS::Assert(false, "Could not find COMPONENTTYPE ", componentType);
}

template<typename T2>
void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<std::vector<T2>>& item)
{
	item.item->clear();
	auto& children = childStr.GetChildren();
	for(auto& childInt : children)
	{
		item.item->emplace_back();
		T2 copyRef;
		SerialNameContainer<T2> copyItem("", &copyRef, "");
		ApplyConversionFunction(*childInt, copyItem);
		item.item->back() = copyRef;
	}
}

void ApplyConversionFunction(const SerializeElem& childItem,
	SerialNameContainer<EventList>& item)
{
	item.item->clear();
	auto& children = childItem.GetChildren();
	for(auto& childStr: children)
	{
		std::string eventName = childStr->Name();
		unsigned i = 0;
		for (; i < EventDirectory::names.size(); ++i)
		{
			if (EventDirectory::names[i] == eventName)
				break;
		}
		if (i == EventDirectory::names.size())
			CONSOLE_ERROR("Event name(", eventName, ") does not exist in the library!");
		else
		{
			auto type = static_cast<EVENTTYPE>(i);
			item.item->emplace_back(COREENGINE_S.GetEventMap()[type]());
			item.item->back()->SetType(type);
			item.item->back()->ConvertSerializedData(*childStr, "");
		}
	}
}

void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<std::vector<AudioInfo>>& item)
{
	item.item->clear();
	auto& children = childStr.GetChildren();
	for(auto& childInt: children)
	{
		item.item->emplace_back();
		item.item->back().ConvertSerializedData(*childInt);
	}
}
void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<std::vector<sBone>>& item)
{
	item.item->clear();
	auto& children = childStr.GetChildren();
	for (auto& childInt : children)
	{
		item.item->emplace_back();
		item.item->back().ConvertSerializedData(*childInt);
	}
}
void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<std::vector<sAnimation>>& item)
{
	item.item->clear();
	auto& children = childStr.GetChildren();
	for (auto& childInt : children)
	{
		item.item->emplace_back();
		item.item->back().ConvertSerializedData(*childInt);
	}
}
void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<std::vector<sChannel>>& item)
{
	item.item->clear();
	auto& children = childStr.GetChildren();
	for (auto& childInt : children)
	{
		item.item->emplace_back();
		item.item->back().ConvertSerializedData(*childInt);
	}
}
void ApplyConversionFunction(const SerializeElem& childStr,
	SerialNameContainer<std::vector<SerialMesh>>& item)
{
	item.item->clear();
	auto& children = childStr.GetChildren();
	for (auto& childInt : children)
	{
		item.item->emplace_back();
		item.item->back().ConvertSerializedData(*childInt);
	}
}


template<typename T2>
void ConvertType(const SerializeElem& elem,
	const std::string& name, std::vector<T2>& vv,
	std::string objName)
{
	auto child = elem.FirstChildElement(name.c_str());
	if (objName.size())
		objName += ": ";
	if (child)
	{
		auto& children = child->GetChildren();
		for(auto&childStr : children)
		{
			bool found = false;
			for (auto& item : vv)
			{
				if (item.name == childStr->Name())
				{
					found = true;
					ApplyConversionFunction(*childStr, item);
					break;
				}
			}
			//if (!found)
			//CONSOLE_ERROR(objName, "Couldn't find property(", childStr->Name(), ") inside ", elem->Name(), " Component!");
		}
	}
}

template<unsigned I1>
struct ConvertExpanderLeaf
{
	template<typename U, typename V>
	ConvertExpanderLeaf(const SerializeElem& elem, const U& names,
		V& tuple, const std::string& name)
	{
		ConvertType(elem, names[I1].data(), std::get<I1>(tuple), name);
	}
};

template<typename T>
struct ConvertExpander
{};

template<unsigned... Indices>
struct ConvertExpander<std::index_sequence<Indices...>> :
	ConvertExpanderLeaf<Indices>...
{
	template<typename U, typename V>
	ConvertExpander(const SerializeElem& elem, const U& names,
		V& tuple, const std::string& name) :
		ConvertExpanderLeaf<Indices>(elem, names, tuple, name)...
	{

	}
};

template<typename U, typename ...Us>
void ExpandConverter(const SerializeElem& elem, const U& names,
	std::tuple<Us...>& tuple, const std::string& name)
{
	using sequence = std::make_index_sequence<sizeof...(Us)>;
	ConvertExpander<sequence> seq(elem, names, tuple, name);
}
void Serializable::ConvertSerializedData(const SerializeElem& elem,
	const std::string& objName)
{
	ExpandConverter(elem, serialTupleContainer.serializedNames,
		serialTupleContainer.serializedItems, objName);
	UpdateSerializables();
}

template<typename T>
struct CheckSaved
{
	bool wasActive = false;
	T content = T();
};

template<typename T>
bool UpdateItemHistory(T& item)
{
	bool returnBool = false;
	static std::map<T*, CheckSaved<T>> saveMap;
	if (ImGui::IsItemActive())
	{
		if (!saveMap[&item].wasActive)
			saveMap[&item].content = item;
		saveMap[&item].wasActive = true;
	}
	else if (saveMap[&item].wasActive)
	{
		if (saveMap[&item].content != item)
		{
			BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY,
				"Updated serializable data" });
			returnBool = true;
		}
		saveMap[&item].wasActive = false;
	}
	return returnBool;
}

bool UpdateItemHistory(StringStruct& item)
{
	bool returnBool = false;
	static std::map<StringStruct*, CheckSaved<std::string>> saveMap;
	if (ImGui::IsItemActive())
	{
		if (!saveMap[&item].wasActive)
			saveMap[&item].content = item.container.data();
		saveMap[&item].wasActive = true;
	}
	else if (saveMap[&item].wasActive)
	{
		if (saveMap[&item].content != item.container.data())
		{
			BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY,
				"Updated serializable data" });
			returnBool = true;
		}
		saveMap[&item].wasActive = false;
	}
	return returnBool;
}
template<typename T>
bool GenerateTypeUI(const std::string&,
	const std::string&, T&)
{
	/*auto returnBool = UpdateItemHistory(item);
	TOOLS::AddToolTip(tooltip);
	return returnBool;*/
	return false;
}

bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, StringStruct& item)
{
	UNREFERENCED_PARAMETER(name);
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputText("", &item.container[0], item.container.size());
	ImGui::PopID();
	if (tooltip.size() && ImGui::IsItemHovered())
		ImGui::SetTooltip(tooltip.c_str());
	ImGui::SameLine();
	bool returnBool = false;
	TOOLS::PushID(UITYPE::DEFAULT);
	UpdateItemHistory(item);
	if (ImGui::Button("..."))
	{
		std::string file{ };
		if (TOOLS::BrowseFolder(file, FILEDIALOGTYPE::OPEN))
		{
			// Clear out the item container
			for (auto& elem : item.container)
				elem = 0;
			TOOLS::MyCopy(file.begin(), file.end(), item.container.begin());
			// Set the last character to be \0
			item.container[file.size()] = '\0';
			BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY,
				"Updated serializable data" });
		}
		returnBool = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Edit"))
	{
		COREENGINE_S.GetEditor()->OpenIDE(item.container.data());
		returnBool = true;
	}
	ImGui::PopID();
	return returnBool;
}

bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, bool& item)
{
	ImGui::SameLine();
	ImGui::Checkbox(name.c_str(), &item);
	auto returnBool = UpdateItemHistory(item);
	TOOLS::AddToolTip(tooltip);
	return returnBool;
}

bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, float& item)
{
  bool returnBool = false;
  if (ImGui::DragFloat(name.c_str(), &item,0.1f)) returnBool = true;
	UpdateItemHistory(item);
	TOOLS::AddToolTip(tooltip);
	return returnBool;
}

bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, int& item)
{
  bool returnBool = false;
  if (ImGui::DragInt(name.c_str(), &item)) returnBool = true;
	UpdateItemHistory(item);
	TOOLS::AddToolTip(tooltip);
	return returnBool;
}
bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, unsigned& item)
{
	bool returnBool = false;
	int ui = item;
	if (ImGui::InputInt(name.c_str(), &ui)) returnBool = true;
	item = ui;
	UpdateItemHistory(item);
	TOOLS::AddToolTip(tooltip);
	return returnBool;
}

bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, COMPONENTTYPE& item)
{
	int componentID = static_cast<int>(item);
	Serializable::DisplayComponentTypes(componentID);
	auto returnBool = UpdateItemHistory(item);
	ImGui::SameLine();
	ImGui::Text(name.c_str());
	item = static_cast<COMPONENTTYPE>(componentID);
	TOOLS::AddToolTip(tooltip);
	return returnBool;
}

bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, Vector2& item)
{
  bool returnBool = false;
	TOOLS::PushID(UITYPE::DEFAULT);
	if (ImGui::DragFloat2(name.c_str(), &item.x)) returnBool = true;
	UpdateItemHistory(item);
	TOOLS::AddToolTip(tooltip);
	ImGui::PopID();
  return returnBool;
}


bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, Vector3& item)
{
  bool returnBool = false;
	TOOLS::PushID(UITYPE::DEFAULT);
  if (ImGui::DragFloat3(name.c_str(), &item.x)) returnBool = true;
	UpdateItemHistory(item);
	TOOLS::AddToolTip(tooltip);
	ImGui::PopID();
	return returnBool;
}
bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, Vector4& item)
{
	bool returnBool = false;
	if (ImGui::ColorEdit4(name.c_str(), &item.x))
		returnBool = true;
	UpdateItemHistory(item);
	TOOLS::AddToolTip(tooltip);
	return returnBool;
}

template<typename T>
bool GenerateTypeUI(const std::string& name,
	const std::string& tooltip, std::vector<T>& vec)
{
	UNREFERENCED_PARAMETER(name);
	UNREFERENCED_PARAMETER(tooltip);
	bool returnBool = false;
	TOOLS::PushID(UITYPE::DEFAULT);
	int currentSize = (int)vec.size();
	ImGui::InputInt("Size", &currentSize);
	if (currentSize < 0) currentSize = 0;
	returnBool = UpdateItemHistory(currentSize);
	ImGui::PopID();
	while (static_cast<unsigned>(currentSize) > vec.size())
	{
		vec.emplace_back();
	}
	while (static_cast<unsigned>(currentSize) < vec.size())
	{
		if (vec.size())
			vec.pop_back();
	}
	for (unsigned i = 0; i < vec.size(); ++i)
	{
		std::stringstream aa;
		aa << i;
		if (!returnBool)
			returnBool = GenerateTypeUI(aa.str().c_str(), "", vec[i]);
		else
			GenerateTypeUI(aa.str(), "", vec[i]);
	}
	return returnBool;
}

bool GenerateTypeUI(const std::string& name,
	const std::string&tooltip, AudioInfo& ss)
{
	ImGui::Separator();
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::Text(name.c_str());
	TOOLS::AddToolTip(tooltip);
	ImGui::PopID();
	ImGui::Separator();
	auto updateRequired = ss.GenerateUI();
	TOOLS::PushID(UITYPE::DEFAULT);
	if (ImGui::Button("Test Sound"))
	{
		ss.StopTestSound();
		ss.TestSound();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop Test Sound"))
		ss.StopTestSound();
	ImGui::PopID();
	return updateRequired.has_value();
}

template<typename T>
void GeneralGenerateTypeUI(std::optional<int>& updateRequired, T& vec,
	int& counter, const std::vector<bool>& asterisks)
{
	for (auto& item : vec)
	{
		if (!item.display) continue;
		auto name = item.name;
		if (asterisks[counter++])
			name += "*";
		ImGui::Text(name.c_str());
		if (item.tooltip.size() && ImGui::IsItemHovered())
			ImGui::SetTooltip(item.tooltip.c_str());
		TOOLS::PushID(UITYPE::DEFAULT);
		if (GenerateTypeUI("", "", item.container))
			updateRequired = counter - 1;
		ImGui::PopID();
	}
}

void GeneralGenerateTypeUI(std::optional<int>& updateRequired,
	std::vector<SerialNameContainer<std::unique_ptr<GeneralCallableFunction>>>& vec,
	int& counter, const std::vector<bool>&)
{
	for (auto& fn : vec)
	{
		if (!fn.display) continue;
		int componentID = fn.componentID;
		ImGui::Text("Component Type");
		Serializable::DisplayComponentTypes(componentID);
		if (fn.tooltip.size() && ImGui::IsItemHovered())
			ImGui::SetTooltip(fn.tooltip.c_str());
		if (componentID == 999)
			componentID = 0;
		//if id has changed
		if (componentID != fn.componentID)
		{
			fn.componentID = componentID;
			//create selection of strings to display
			auto tempComp = COMPONENTMAP[static_cast<COMPONENTTYPE>(componentID + 1)]();
			tempComp->AddSerializeData();
			fn.componentFunctionNames.clear();
			for (auto& fnItem : tempComp->functionList)
				fn.componentFunctionNames.emplace_back(fnItem->name);
			fn.functionID = 0;
			if (fn.componentFunctionNames.size())
				*fn.item = tempComp->functionList[fn.functionID]->Clone();
		}
		if (fn.componentFunctionNames.size())
		{
			std::vector<const char*> args;
			// Push everything but the first value
			for (unsigned i = 0; i < fn.componentFunctionNames.size(); ++i)
				args.push_back(fn.componentFunctionNames[i].data());
			char** charArgs = const_cast<char**>(&args[0]);

			ImGui::Text("Function Name");
			TOOLS::PushID(UITYPE::DEFAULT);
			int oldID = fn.functionID;
			ImGui::Combo("", &oldID, charArgs,
				int(fn.componentFunctionNames.size()), int(fn.componentFunctionNames.size()));
			ImGui::PopID();
			if (oldID != fn.functionID)
			{
				fn.functionID = oldID;
				auto tempComp = COMPONENTMAP[static_cast<COMPONENTTYPE>(fn.componentID + 1)]();
				tempComp->AddSerializeData();
				*fn.item = tempComp->functionList[fn.functionID]->Clone();
			}
			if ((*fn.item).get())
			{
				if (ImGui::TreeNode("Properties"))
				{
					auto isUpdated = (*fn.item)->GenerateUI();
					if (!updateRequired && isUpdated)
						updateRequired = counter - 1;
					ImGui::TreePop();
				}
			}
		}
	}
}

void GeneralGenerateTypeUI(std::optional<int>& bb, std::vector<SerialNameContainer<EventList>>& vec,
	int& counter, const std::vector<bool>& asterisks)
{
	static auto upArrow = GFX_S.GetTexture("Resources/Sprites/Editor/uparrow.dds");
	static auto downArrow = GFX_S.GetTexture("Resources/Sprites/Editor/downarrow.dds");
	static auto deleteButton = GFX_S.GetTexture("Resources/Sprites/Editor/delete.dds");
	for (auto& item : vec)
	{
		if (!item.display) continue;
		int size = (int)item.container.size();
		std::stringstream ss;
		if(asterisks[counter++])
			ss << item.name << "*" << "(" << size << ")";
		else
			ss << item.name << "(" << size << ")";
		if (ImGui::TreeNode(item.name.c_str(), ss.str().c_str()))
		{
			TOOLS::PushID(UITYPE::DEFAULT);
			ImGui::InputInt("Size", &size);
			TOOLS::AddToolTip(item.tooltip);
			ImGui::PopID();
			//TOOLS::PushID(UITYPE::DEFAULT);
			//if (ImGui::Button("Cut"))
			//{
			//	/*COREENGINE_S.Config.copiedList.clear();
			//	for (unsigned k = 0; k < item.container.size(); ++k)
			//		MODEL_S.Config.copiedList.emplace_back(item.container[k]->Clone());
			//	item.container.clear();*/
			//}
			//ImGui::PopID();
			//ImGui::SameLine();
			//TOOLS::PushID(UITYPE::DEFAULT);
			//if (ImGui::Button("Copy"))
			//{
			//	/*MODEL_S.Config.copiedList.clear();
			//	for (unsigned k = 0; k < item.container.size(); ++k)
			//		MODEL_S.Config.copiedList.emplace_back(item.container[k]->Clone());*/
			//}
			//ImGui::PopID();
			//ImGui::SameLine();
			//TOOLS::PushID(UITYPE::DEFAULT);
			///*if (ImGui::Button("Paste") && MODEL_S.Config.copiedList.size())
			//{
			//	item.container.clear();
			//	for (unsigned k = 0; k < MODEL_S.Config.copiedList.size(); ++k)
			//	{
			//		item.container.emplace_back(MODEL_S.Config.copiedList[k]->Clone());
			//	}
			//	size = item.container.size();
			//}*/
			//ImGui::PopID();
			//ImGui::SameLine();
			//TOOLS::PushID(UITYPE::DEFAULT);
			///*if (ImGui::Button("Append to back") && MODEL_S.Config.copiedList.size())
			//{
			//	for (unsigned k = 0; k < MODEL_S.Config.copiedList.size(); ++k)
			//	{
			//		item.container.emplace_back(MODEL_S.Config.copiedList[k]->Clone());
			//	}
			//	size = item.container.size();
			//}*/
			//ImGui::PopID();
			if (size < 0) size = 0;
			while (static_cast<unsigned>(size) > item.container.size())
			{
				item.container.emplace_back();
				item.container.back() = COREENGINE_S.GetEventMap()[static_cast<EVENTTYPE>(0)]();
				item.container.back()->SetType(static_cast<EVENTTYPE>(0));
				item.container.back()->UpdateSerializables();
				auto updated = item.container.back()->GenerateUI();
				if (!bb && updated)
					bb = counter - 1;
			}
			while (static_cast<unsigned>(size) < item.container.size())
			{
				if (item.container.size())
					item.container.pop_back();
			}
			for (unsigned k = 0; k < item.container.size(); ++k)
			{
				ImGui::Separator();
				auto& eventItem = item.container[k];
				std::stringstream str;
				str << "Event " << k;
				int& eventEnum = eventItem->eventNum;
				int oldNum = eventEnum;
				size_t sz = EventDirectory::names.size();
				static std::vector<const char*> args;
				if (!args.size())
				{
					// Push everything but the first value
					for (unsigned i = 0; i != sz; ++i)
						args.push_back(EventDirectory::names[i].data());
				}
				static constexpr float miniIconSize = 13.f;

				char** charArgs = const_cast<char**>(&args[0]);
				ImGui::Text("Component Type");
				ImGui::SameLine();
				TOOLS::PushID(UITYPE::DEFAULT);
				bool breakNow = false;
				if (ImGui::ImageButton((ImTextureID)upArrow->GetID(), { miniIconSize, miniIconSize }))
				{
					if (k)
						std::swap(eventItem, item.container[k - 1]);
					breakNow = true;
				}
				ImGui::PopID();
				ImGui::SameLine();
				TOOLS::PushID(UITYPE::DEFAULT);
				if (ImGui::ImageButton((ImTextureID)downArrow->GetID(), { miniIconSize, miniIconSize }))
				{
					if (k != item.container.size() - 1)
						std::swap(eventItem, item.container[k + 1]);
					breakNow = true;
				}
				ImGui::PopID();
				ImGui::SameLine();
				//delete button
				TOOLS::PushID(UITYPE::DEFAULT);
				if (ImGui::ImageButton((ImTextureID)deleteButton->GetID(), { miniIconSize, miniIconSize }))
				{
					size_t sizeMinusOne = item.container.size() - 1;
					for (size_t l = k; l < sizeMinusOne; ++l)
					{
						std::swap(item.container[l], item.container[l + 1]);
					}
					item.container.pop_back();
					size = (int)item.container.size();
					breakNow = true;
				}
				ImGui::PopID();
				if (breakNow) break;

				TOOLS::PushID(UITYPE::DEFAULT);
				ImGui::Combo(str.str().c_str(), &eventEnum, charArgs, (int)EventDirectory::names.size(),
					(int)EventDirectory::names.size());
				ImGui::PopID();
				if (oldNum != eventEnum)
				{
					oldNum = eventEnum;
					eventItem = COREENGINE_S.GetEventMap()[static_cast<EVENTTYPE>(eventEnum)]();
					eventItem->SetType(static_cast<EVENTTYPE>(oldNum));
					eventItem->eventNum = oldNum;
					eventItem->UpdateSerializables();
				}
				std::stringstream funcName;
				funcName << "Settings";
				if (static_cast<EVENTTYPE>(oldNum) == EVENTTYPE::ACTIVATEFUNCTION)
				{
					auto activateName = static_cast<ActivateFunction*>(eventItem.get())->callFunction.get();
					if (activateName)
						funcName << "(" << activateName->name << ")";
				}
				TOOLS::PushID(UITYPE::TREEHEADER);
				if (ImGui::TreeNode("Settings", funcName.str().c_str()))
				{
					auto updated = eventItem->GenerateUI();
					if (!bb)
						bb = counter - 1;
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		counter++;
	}
}


template<unsigned I1>
struct UIExpanderLeaf
{
	template<typename T>
	UIExpanderLeaf(std::optional<int>& updateRequired, T& tuple, int& counter,
		const std::vector<bool>& asterisks)
	{
		GeneralGenerateTypeUI(updateRequired, std::get<I1>(tuple), counter, asterisks);
	}
};

template<typename T>
struct UIExpander
{};

template<unsigned... Indices>
struct UIExpander<std::index_sequence<Indices...>> :
	UIExpanderLeaf<Indices>...
{
	template<typename T>
	UIExpander(std::optional<int>& updateRequired, T& tuple, int& counter,
		const std::vector<bool>& asterisks) :
		UIExpanderLeaf<Indices>(updateRequired, tuple, counter, asterisks)...
	{

	}
};

template<typename ...Us>
void ExpandUI(std::optional<int>& updateRequired, std::tuple<Us...>& tuple, int& counter,
	const std::vector<bool>& asterisks)
{
	using sequence = std::make_index_sequence<sizeof...(Us)>;
	UIExpander<sequence> seq(updateRequired, tuple, counter, asterisks);
}

std::optional<int> Serializable::GenerateUI()
{
	counter = 0;
	std::optional<int> updateRequired;
	ExpandUI(updateRequired, serialTupleContainer.serializedItems, counter, asterisks);
	if (updateRequired)
		UpdateComponentData();
	return updateRequired;
}

void Serializable::DisplayComponentTypes(int& componentID)
{
	static auto  vec_names = ComponentDirectory::names;
	static const size_t sz = vec_names.size() - 1;
	static std::vector<const char*> args;
	if (!args.size())
	{
		// Push everything but the first value
		for (unsigned i = 1; i <= sz; ++i)
			args.push_back(vec_names[i].data());
	}
	char** charArgs = const_cast<char**>(&args[0]);
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::Combo("", &componentID, charArgs, int(sz), int(sz));
	ImGui::PopID();
}
void Serializable::UpdateAudioInfo()
{
	auto& ss = std::get<std::vector<SerialNameContainer<std::vector<AudioInfo>>>>(serialTupleContainer.serializedItems);
	for (auto& item : ss)
	{
		for (auto&item2 : item.container)
			item2.UpdateSerializables();
	}
}

void Serializable::UpdateSerialMeshInfo()
{
	auto& ss = std::get<std::vector<SerialNameContainer<std::vector< SerialMesh>>>>(serialTupleContainer.serializedItems);
	for (auto& item : ss)
	{
		for (auto&item2 : item.container)
		{
			item2.UpdateSerializables();
			item2.UpdateSerializableInfo < std::vector<sBone>>();
		}
	}
}

void Serializable::UpdateSerialAnimInfo()
{
	auto& ss = std::get<std::vector<SerialNameContainer<std::vector< sAnimation>>>>(serialTupleContainer.serializedItems);
	for (auto& item : ss)
	{
		for (auto&item2 : item.container)
		{
			item2.UpdateSerializables();
			item2.UpdateSerializableInfo < std::vector<sChannel>>();
		}
	}
}

template<typename T>
void DifferenceExpander(const std::vector <SerialNameContainer<T>>&t1, 
	const std::vector <SerialNameContainer<T>>&t2, int& pos, bool& bb)
{
	if (pos < 0) return;
	pos -= (int)t1.size();
	if (pos < 0)
	{
		pos += (int)t1.size();
		if (*t1[pos].item != *t2[pos].item)
			bb = true;
		else bb = false;
		pos -= (int)t1.size();
	}
}

template<typename ...Us>
bool ExpandCheckDifferences(const SerialTupleContainer<Us...>& tuple, 
	const SerialTupleContainer<Us...>& tuple2, int& counter)
{
	bool returnBool = false;
	(DifferenceExpander<Us>(std::get<std::vector<SerialNameContainer<Us>>>(tuple.serializedItems),
		std::get<std::vector<SerialNameContainer<Us>>>(tuple2.serializedItems), counter, returnBool),...);
	return returnBool;

}

bool Serializable::CheckDifferences(const Serializable & ss, int pos) const
{
	return ExpandCheckDifferences(ss.serialTupleContainer, serialTupleContainer, pos);
}
template<typename T>
void ExpandModified(Serializable& curr, const std::vector <SerialNameContainer<T>>& otherVec)
{
	UNREFERENCED_PARAMETER(otherVec);
	auto& vec = std::get<std::vector<SerialNameContainer<T>>>(curr.serialTupleContainer.serializedItems);

	if constexpr (!std::is_same<T, EventList>())
	{
		for (size_t i = 0; i < vec.size(); ++i)
		{
			if constexpr (std::is_copy_assignable<T>())
			{
				//std::cout << ConvertTypeToName<T>()<<std::endl;
				if (!curr.asterisks[curr.counter])
				{
					*vec[i].item = *otherVec[i].item;
				}
			}
			++curr.counter;
		}
	}
	else
		curr.counter += vec.size();
}
template<typename ...Us>
void ExpandModifiedHelper(Serializable& tuple, const SerialTupleContainer<Us...>& tuple2)
{
	(ExpandModified<Us>(tuple, std::get<std::vector<SerialNameContainer<Us>>>(tuple2.serializedItems)),...);
}


void Serializable::CopyUnmodifiedVariables(const Serializable & other) 
{
	counter = 0;
	ExpandModifiedHelper(*this, other.serialTupleContainer);
	UpdateSerializables();
}

