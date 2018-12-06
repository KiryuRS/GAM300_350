#include "stdafx.h"

std::optional<SerializeElem> Serializer::CreateElem(size_t pos, const std::string & partial)
{
	auto objects = TOOLS::SplitString(partial, ", ");
	if (objects.size() > 1)
	{
		auto var = GetVariable(pos, objects[0]);
		SerializeElem returnElem(var.first);
		if (var.second.size())
			returnElem.SetText(var.second);
		//get attributes
		for (unsigned i = 1; i < objects.size(); ++i)
		{
			auto var2 = GetVariable(0, objects[i]);
			returnElem.SetAttribute(var2.first, var2.second);
		}
		return returnElem;
	}
	else
	{
		auto var = GetVariable(pos, partial);
		SerializeElem returnElem(var.first);
		if (var.second.size())
			returnElem.SetText(var.second);
		//get attributes
		for (unsigned i = 1; i < objects.size(); ++i)
		{
			auto var2 = GetVariable(pos, objects[1]);
			returnElem.SetAttribute(var2.first, var2.second);
		}
		return returnElem;
	}
	return std::optional<SerializeElem>();
}

SERIALRESULT Serializer::LoadFile(const std::string & filename)
{
	std::ifstream file(filename);
	if (file.fail())
		return SERIALRESULT::MISSINGFILE;

	std::string partial;
	std::vector<SerializeElem*> elems;
	elems.emplace_back(nullptr);
	int prevSpaces = 0;
	while (std::getline(file, partial))
	{
		auto numSpaces = NumSpaces(partial);
		if (numSpaces > prevSpaces)
		{
			prevSpaces = numSpaces;
		}
		else if (numSpaces == prevSpaces)
		{
			elems.pop_back();
		}
		else
		{
			while (prevSpaces != numSpaces)
			{
				elems.pop_back();
				prevSpaces -= 2;
			}
			elems.pop_back();
		}
		auto newElem = CreateElem(numSpaces, partial);
		if (newElem)
		{
			if (numSpaces == 0)
				elems.emplace_back(&NewElement(std::move(newElem.value())));
			else
				elems.emplace_back(&elems.back()->InsertChild(std::move(newElem.value())));
		}
	}

	if (!file.eof())
	{
		std::cout << "Error: Not end of file while loading, something went wrong\n";
		return SERIALRESULT::INVALIDFILE;
	}
	file.close();
	return SERIALRESULT::SUCCESS;
}
