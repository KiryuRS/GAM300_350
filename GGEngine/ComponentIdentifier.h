#pragma once
#include "Component.h"

class IdentifierC : public Component
{
public:
	// unique "id" for each of the component
	std::string name;

	IdentifierC() = default;
	void AddSerializeData(LuaScript *state = nullptr) override;
	void UpdateID(std::string newName) { name = newName; }
	static constexpr auto GetType() { return COMPONENTTYPE::IDENTIFIER; }
};