#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include <optional>
#include "Animation.h"

enum class ELEMRESULT{SUCCESS, NOTFOUND};

class SerializeElem
{
	std::string name;
	std::string text;
	std::unordered_map<std::string, std::string> attributes;
	std::vector<std::unique_ptr<SerializeElem>> children;
	template<typename T>
	inline T ConvertToType(const std::string& str) const
	{
		static_assert(1, "Type conversion not supported!");
	}
	template<>
	inline float ConvertToType<float>(const std::string& str) const
	{
		return stof(str);
	}
	template<>
	inline int ConvertToType<int>(const std::string& str) const
	{
		return stoi(str);
	}
	template<>
	inline unsigned ConvertToType<unsigned>(const std::string& str) const
	{
		return stoul(str);
	}
	template<>
	inline std::string ConvertToType<std::string>(const std::string& str) const
	{
		return str;
	}
	template<>
	inline bool ConvertToType<bool>(const std::string& str) const
	{
		auto val = stoi(str);
		if (val)
			return true;
		return false;
	}
	template<>
	Node ConvertToType<Node>(const std::string& str) const
	{
		Node returnNode;
		std::vector<Node*> nodeStack;
		nodeStack.emplace_back(&returnNode);
		size_t currIndex = 0;
		UNREFERENCED_PARAMETER(currIndex);

		std::string currString;
		auto endPos = str.find('(');
		auto midPos = str.find('|');
		nodeStack.back()->mMappingIndex = stoul(str.substr(0, midPos));
		nodeStack.back()->name = str.substr(midPos + 1, endPos - midPos - 1);

		for (size_t i = endPos + 1; i < str.size(); ++i)
		{
			if (str[i] == '(')
			{
				nodeStack.emplace_back(&nodeStack.back()->children.back());
				auto middlePos = currString.find('|');
				nodeStack.back()->mMappingIndex = stoul(currString.substr(0, middlePos));
				nodeStack.back()->name = currString.substr(middlePos + 1, currString.size() - middlePos - 1);
				currString.clear();
			}
			else if (str[i] == ')')
			{
				nodeStack.pop_back();
			}
			else
			{
				if (str[i] == '|')
				{
					nodeStack.back()->children.emplace_back();
				}
				currString += str[i];
			}
		}

		return std::move(returnNode);
	}
public:
	SerializeElem(const std::string& newName):
		name(newName)
	{}
	template<typename T>
	inline void SetText(const T& item)
	{
		std::stringstream ss;
		ss << item;
		text = ss.str();
	}
	void SetText(const Node& item);
	template<typename T>
	inline void SetAttribute(const std::string& att, T item)
	{
		std::stringstream ss;
		ss << item;
		attributes[att] = ss.str();
	}
	template<typename T>
	inline ELEMRESULT QueryAttribute(const std::string& str, T& assign) const
	{
		auto iter = attributes.find(str);
		if (iter != attributes.end())
		{
			assign = ConvertToType<T>(iter->second);
			return ELEMRESULT::SUCCESS;
		}
		return ELEMRESULT::NOTFOUND;
	}
	template<typename T>
	inline ELEMRESULT QueryText(T& assign) const
	{
		if (text.size())
		{
			assign = ConvertToType<T>(text);
			return ELEMRESULT::SUCCESS;
		}
		return ELEMRESULT::NOTFOUND;
	}
	inline SerializeElem& InsertChild(const std::string&nam)
	{
		return *children.emplace_back(std::make_unique< SerializeElem>(nam));
	}
	inline SerializeElem& InsertChild(SerializeElem&&obj)
	{
		return *children.emplace_back(std::make_unique<SerializeElem>(std::move(obj)));
	}
	void AppendElem(unsigned numSpaces, std::ofstream& file) const 
	{
		for (unsigned i = 0; i < numSpaces; ++i)
		{
			file << " ";
		}
		file << name << ": " << text;
		for (const auto& item : attributes)
		{
			file << ", " << item.first << ": " << item.second;
		}
		file << "\n";
		for (auto& item : children)
		{
			item->AppendElem(numSpaces + 2, file);
		}
	}
	inline SerializeElem* FirstChildElement()
	{
		if (children.size())
			return children[0].get();
		return nullptr;
	}
	inline const SerializeElem* FirstChildElement() const
	{
		if (children.size())
			return children[0].get();
		return nullptr;
	}
	inline SerializeElem* FirstChildElement(const std::string& str)
	{
		for (auto& item : children)
		{
			if (item->name == str)
				return item.get();
		}
		return nullptr;
	}
	inline const SerializeElem* FirstChildElement(const std::string& str) const
	{
		for (auto& item : children)
		{
			if (item->name == str)
				return item.get();
		}
		return nullptr;
	}
	inline std::string Name() const
	{
		return name;
	}
	inline auto& GetChildren()
	{
		return children;
	}
	inline const auto& GetChildren() const
	{
		return children;
	}
};