#include "stdafx.h"

void IdentifierC::AddSerializeData(LuaScript * state)
{
	AddSerializable("ID", name, "Unique \"Identifier\" for each entity", state);
	AddCallableFunction("Update_ID", &IdentifierC::UpdateID, {}, state);
}
