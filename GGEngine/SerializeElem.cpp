#include "stdafx.h"

std::string RecurseNodeText(const Node& item)
{
	std::stringstream ss;
	ss << item.mMappingIndex << "|" << item.name;
	ss << "(";
	for (const auto& node : item.children)
	{
		ss << RecurseNodeText(node);
	}
	ss << ")";
	return ss.str();
}

void SerializeElem::SetText(const Node & item)
{
	text = RecurseNodeText(item);
}
