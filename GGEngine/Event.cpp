#include "stdafx.h"

std::unique_ptr<Event> Event::Clone() const
{
	auto returnptr = COREENGINE_S.GetEventMap()[type]();
	return std::move(returnptr);
}


void CameraShake::Activate()
{
	std::cout << "Camera is supposed to shake here!\n";
	//owner->entityList->GetCamera()->ShakeCamera(intensity, shakeTime);
}

void CameraShake::Update(float dt)
{
	if (shakeTime < dt)
		EndEvent();
	else
		shakeTime -= dt;
}

void ActivateFunction::Activate()
{
	if (!targetID.size())
		callFunction->ActivateFunction(*owner);
	else
	{
		auto ent = owner->entityList->FindEntity(targetID);
		if (ent)
			callFunction->ActivateFunction(*ent);
	}
	EndEvent();
}
