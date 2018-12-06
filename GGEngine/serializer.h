#pragma once
#include "SerializeElem.h"

enum class SERIALRESULT{ SUCCESS, MISSINGFILE, INVALIDFILE };

class Serializer 
{
	std::vector<std::unique_ptr<SerializeElem>> elements;
	int NumSpaces(const std::string& str) const
	{
		int returnNum = 0;
		auto iter = str.begin();
		while (iter != str.end())
		{
			if (*iter != ' ')
				break;
			++returnNum;
			++iter;
		}
		return returnNum;
	}
	std::pair<std::string, std::string> GetVariable(size_t pos, const std::string& str)
	{
		std::pair<std::string, std::string> returnPair;
		auto iter = str.find(": ");
		returnPair.first = str.substr(pos, iter - pos);
		returnPair.second = str.substr(iter + 2, str.size() - iter - 2);
		return returnPair;
	}
	std::optional<SerializeElem> CreateElem(size_t pos, const std::string & partial);
public:
	std::string SerialResults[3]{ "SUCCESS","MISSINGFILE","INVALIDFILE" };
	inline SerializeElem& NewElement(const std::string& nam)
	{
		return *elements.emplace_back(std::make_unique<SerializeElem>(nam));
	}
	inline SerializeElem& NewElement(SerializeElem&& obj)
	{
		return *elements.emplace_back(std::make_unique<SerializeElem>(std::move(obj)));
	}
	SERIALRESULT LoadFile(const std::string& filename);
	inline SerializeElem* FirstChild()
	{
		if (!elements.size())
			return nullptr;
		else
			return elements.front().get();
	}
	inline const SerializeElem* FirstChild() const
	{
		if (!elements.size())
			return nullptr;
		else
			return elements.front().get();
	}
	SERIALRESULT SaveFile(const std::string& filename)
	{
		std::ofstream file(filename);
		for (auto& item : elements)
			item->AppendElem(0, file);
		file.close();
		return SERIALRESULT::SUCCESS;
	}
	inline SerializeElem* FirstChildElement(const std::string& str)
	{
		for (auto& item : elements)
		{
			if (item->Name() == str)
				return item.get();
		}
		return nullptr;
	}
	inline const SerializeElem* FirstChildElement(const std::string& str) const
	{
		for (auto& item : elements)
		{
			if (item->Name() == str)
				return item.get();
		}
		return nullptr;
	}
};