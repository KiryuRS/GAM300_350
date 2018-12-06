/******************************************************************************/
/*!
\File	   	       PlayerC.h
\Primary Author    Ng Yan Fei 100%
\Project Name      Nameless


Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once
#include "Component.h"

class PlayerC : public Component
{
public:
	PlayerC();
	~PlayerC();

	void Initialize() override;
	void Update(float dt);
	static constexpr auto GetType() { return COMPONENTTYPE::PLAYER; }
	void AddSerializeData(LuaScript* state = nullptr) override;
};
