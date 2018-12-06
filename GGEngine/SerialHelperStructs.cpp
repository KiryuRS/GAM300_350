#include "stdafx.h"

std::array<std::string, 25> SerialTupleContainer<ALLSERIALIZEDTYPES>::serializedNames = { ALLSERIALIZEDNAMES };

void UpdateSerializableType(SerialNameContainer<std::string>& item)
{
	unsigned i = 0;
	unsigned strSize = (unsigned)item.item->size();
	for (; i < strSize; ++i)
		item.container.container[i] = (*item.item)[i];
	item.container.container[i] = 0;
}

void UpdateSerializableType(SerialNameContainer<
	std::vector<std::string>>&item)
{
	item.container.clear();
	for (unsigned i = 0; i < item.item->size(); ++i)
	{
		item.container.emplace_back();
		unsigned strSize = (unsigned)(*item.item)[i].size();
		unsigned j = 0;
		for (; j < strSize; ++j)
		{
			item.container.back().container[j] = (*item.item)[i][j];
		}
		item.container.back().container[j] = 0;
	}
}

void UpdateSerializableType(
	SerialNameContainer<CallableFunctionPtr>& item)
{
	if (item.item->get())
		(*item.item)->UpdateSerializables();
}

void UpdateSerializableType(SerialNameContainer<EventList>& item)
{
	item.container.clear();
	for (unsigned i = 0; i < item.item->size(); ++i)
	{
		item.container.emplace_back((*item.item)[i]->Clone());
		item.container.back()->eventNum = static_cast<int>((*item.item)[i]->GetType());
	}
}

void UpdateSerializableType(SerialNameContainer<std::vector<AudioInfo>>& item)
{
	item.container.clear();
	for (unsigned i = 0; i < item.item->size(); ++i)
	{
		item.container.emplace_back((*item.item)[i]);
		item.container.back().UpdateSerializables();
	}
}

void UpdateSerializableType(SerialNameContainer<std::vector<SerialMesh>>& item)
{
	item.container.clear();
	item.container.reserve(item.item->size());
	for (unsigned i = 0; i < item.item->size(); ++i)
	{
		item.container.emplace_back((*item.item)[i]);
		for (auto& item2 : item.container.back().bones)
			item2.UpdateSerializables();
		item.container.back().UpdateSerializables();
		item.container.back().UpdateSerializableInfo<std::vector<sBone>>();
	}
}

void UpdateSerializableType(SerialNameContainer<std::vector<sBone>>& item)
{
	item.container.clear();
	for (unsigned i = 0; i < item.item->size(); ++i)
	{
		item.container.emplace_back((*item.item)[i]);
		item.container.back().UpdateSerializables();
	}
}
void UpdateSerializableType(SerialNameContainer<std::vector<sAnimation>>& item)
{
	item.container.clear();
	for (unsigned i = 0; i < item.item->size(); ++i)
	{
		item.container.emplace_back((*item.item)[i]);
		item.container.back().UpdateSerializables();
		item.container.back().UpdateSerializableInfo<std::vector<sChannel>>();
	}
}

void UpdateSerializableType(SerialNameContainer<std::vector<sChannel>>& item)
{
	item.container.clear();
	for (unsigned i = 0; i < item.item->size(); ++i)
	{
		item.container.emplace_back((*item.item)[i]);
		item.container.back().UpdateSerializables();
	}
}


void UpdateData(SerialNameContainer<std::string>& item)
{
	*item.item = item.container.container.data();
}
void UpdateData(SerialNameContainer<std::vector<std::string>>& item)
{
	item.item->clear();
	for (unsigned i = 0; i < item.container.size(); ++i)
	{
		item.item->emplace_back();
		item.item->back() = &item.container[i].container[0];
	}
}
void UpdateData(SerialNameContainer<CallableFunctionPtr>& item)
{
	if (item.item->get())
		(*item.item)->UpdateComponentData();
}
void UpdateData(SerialNameContainer<EventList>& item)
{
	item.item->clear();
	for (unsigned i = 0; i < item.container.size(); ++i)
		item.item->emplace_back(item.container[i]->Clone());
}
void UpdateData(SerialNameContainer<std::vector<AudioInfo>>& item)
{
	item.item->clear();
	for (unsigned i = 0; i < item.container.size(); ++i)
	{
		item.item->emplace_back(item.container[i]);
		item.item->back().UpdateSerializables();
	}
}
void UpdateData(SerialNameContainer<std::vector<SerialMesh>>& item)
{
	item.item->clear();
	for (unsigned i = 0; i < item.container.size(); ++i)
	{
		item.item->emplace_back(item.container[i]);
		item.item->back().UpdateSerializables();
	}
}
void UpdateData(SerialNameContainer<std::vector<sBone>>& item)
{
	item.item->clear();
	for (unsigned i = 0; i < item.container.size(); ++i)
	{
		item.item->emplace_back(item.container[i]);
		item.item->back().UpdateSerializables();
	}
}
void UpdateData(SerialNameContainer<std::vector<sChannel>>& item)
{
	item.item->clear();
	for (unsigned i = 0; i < item.container.size(); ++i)
	{
		item.item->emplace_back(item.container[i]);
		item.item->back().UpdateSerializables();
	}
}
void UpdateData(SerialNameContainer<std::vector<sAnimation>>& item)
{
	item.item->clear();
	for (unsigned i = 0; i < item.container.size(); ++i)
	{
		item.item->emplace_back(item.container[i]);
		item.item->back().UpdateSerializables();
	}
}