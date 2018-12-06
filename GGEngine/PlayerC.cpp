/******************************************************************************/
/*!
\File	   	       PlayerC.cpp
\Primary Author    Ng Yan Fei 100%
\Project Name      Nameless


Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "stdafx.h"
#include "PlayerC.h"

PlayerC::PlayerC()
{
	
}

void PlayerC::AddSerializeData(LuaScript* state)
{

  UNREFERENCED_PARAMETER(state);
}

PlayerC::~PlayerC()
{
}

void PlayerC::Initialize()
{
	owner->entityList->player = owner;
}

void PlayerC::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}