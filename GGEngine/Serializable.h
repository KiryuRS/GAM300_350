#pragma once
#include <vector>
#include "Vector.h"
#include "SerialHelperStructs.h"
#include "MetaBoilerplates.h"
struct AudioInfo;
class SerializeElem;

class Serializable
{
	bool minimized;
public:
	int counter = 0;
	std::vector<bool> asterisks;
	SerialTupleContainer<ALLSERIALIZEDTYPES>  serialTupleContainer;
	Serializable();
	Serializable(const Serializable&) {}
	Serializable& operator=(const Serializable&) { return *this; }
	template<typename T>
	SerialNameContainer<T>& AddSerializable(const std::string& title, T& value, const std::string& tooltip = "")
	{
		auto& containerVec = std::get<std::vector<SerialNameContainer<T>>>(serialTupleContainer.serializedItems);
		auto item = std::find_if(containerVec.begin(), containerVec.end(), [&](SerialNameContainer<T>& container)
		{
			if (container.name == title)
				return true;
			return false;
		});
		if (item != containerVec.end()) return *item;
		containerVec.emplace_back(title, &value, tooltip);
		asterisks.emplace_back(false);
		return containerVec.back();
	}

	//updates the gui data
	void UpdateSerializables();
	//update the inside component data
	void UpdateComponentData();

	template<typename T>
	void UpdateComponentData(const T& item)
	{
		auto& vec = std::get<std::vector<SerialNameContainer<T>>>(serialTupleContainer.serializedItems);
		for (auto& vecItem : vec)
		{
			if (vecItem.item == &item)
			{
				UpdateData(vecItem);
				break;
			}
		}
	}

	template<typename T>
	void UpdateSerializables(const T& item)
	{
		auto& vec = std::get<std::vector<SerialNameContainer<T>>>(serialTupleContainer.serializedItems);
		for (auto& vecItem : vec)
		{
			if (vecItem.item == &item)
			{
				UpdateSerializableType(vecItem);
				break;
			}
		}
	}
	template<typename T>
	auto GetData(const std::string& str) const
	{
		auto& vec = std::get<std::vector<SerialNameContainer<T>>>(serialTupleContainer.serializedItems);
		for (auto& vecItem : vec)
		{
			if (vecItem.name == str)
			{
				return *vecItem.item;
			}
		}
		return T();
	}
	template<typename T>
	void SetData(const std::string& str, T newData)
	{
		auto& vec = std::get<std::vector<SerialNameContainer<T>>>(serialTupleContainer.serializedItems);
		for (auto& vecItem : vec)
		{
			if (vecItem.name == str)
			{
				*vecItem.item = newData;
				return;
			}
		}
	}
	void Serialize(SerializeElem&) const;
	virtual void ConvertSerializedData(const SerializeElem&, const std::string& objName = "");
	//returns the ID number of a modified variable, if it exists
	virtual std::optional<int> GenerateUI();
	static void DisplayComponentTypes(int&);
	void UpdateAudioInfo();
	void UpdateSerialMeshInfo();
	void UpdateSerialAnimInfo();
	template<typename T>
	void UpdateSerializableInfo()
	{
		auto& ss = std::get<std::vector<SerialNameContainer<T>>>(serialTupleContainer.serializedItems);
		for (auto& item : ss)
		{
			for (auto&item2 : item.container)
				item2.UpdateSerializables();
		}
	}
	template<typename T>
	void RemoveSerializable(T& obj)
	{
		auto& ss = std::get<std::vector<SerialNameContainer<T>>>(serialTupleContainer.serializedItems);
		auto iter = ss.begin();
		while (iter != ss.end())
		{
			if (iter->item == &obj)
			{
				ss.erase(iter);
				return;
			}
			++iter;
		}
	}


	bool CheckDifferences(const Serializable& ss, int pos) const;

	void ApplyAsterisks(Serializable& ss) const
	{
		for (unsigned i = 0; i < asterisks.size(); ++i)
		{
			bool bb = CheckDifferences(ss, i);
			if (bb)
			{
				ss.asterisks[i] = true;
			}
		}
	}

	void CopyUnmodifiedVariables(const Serializable& other);


	template<typename T, typename U>
	void IndexExpander(U& obj, const std::vector <SerialNameContainer<T>>&t1,
		int& pos, int& finalIndex)
	{
		if (finalIndex != -1) return;
		if constexpr (std::is_same<T, U>())
		{
			for (const auto& item : t1)
			{
				if (item.item == &obj)
				{
					finalIndex = pos;
					break;
				}
				++pos;
			}
		}
		else
			pos += t1.size();
	}
	template<typename T, typename ...Us>
	int ExpandIndices(T& obj, const SerialTupleContainer<Us...>& tuple)
	{
		int currIndex = 0;
		int finalIndex = -1;
		(IndexExpander<Us>(obj, std::get<std::vector<SerialNameContainer<Us>>>(tuple.serializedItems),
			currIndex,finalIndex), ...);
		return finalIndex;
	}

	template<typename T>
	int GetAsteriskIndex(T& obj)
	{
		return ExpandIndices(obj, serialTupleContainer);
	}
	virtual ~Serializable() {}
};