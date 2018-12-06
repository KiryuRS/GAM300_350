/******************************************************************************/
/*!
\File	   	       SoundC.h
\Primary Author    Ng Yan Fei 100%
\Project Name      Nameless


Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once
#include "Component.h"

class SoundC : public Component
{
	AudioSystem& audio;
	std::vector<AudioInfo> bgmSounds, sfxSounds, lsfxSounds;
	bool startBGM, startLSFX;
	int startBGMID, startLSFXID;
	float fadeDuration;

public:
	SoundC();
	~SoundC();

	void Initialize() override;
	void Update(float dt);
	void PlaySFX(int);
	void PlayBGM(int);
	void PlayLSFX(int);
	void StopBGM();
	void StopSFX(bool justLSFX = false);
	void StopLSFX(int);
	void SetVolume(float sfxVol, float bgmVol);
	void Pause(bool justBGM = false);
	void Resume();
	float GetSFXVolume(int id = -1);
	float GetBGMVolume(int id = -1);
	float GetLSFXVolume(int id = -1);
	void SetSFXVolume(float, int id = -1);
	void SetLSFXVolume(float, int id = -1);
	void SetBGMVolume(float, int id = -1);
	static constexpr auto GetType() { return COMPONENTTYPE::SOUND; }
	AudioSystem& GetAudioSystem();


	void FadeInLSFX(int id, float seconds);
	void FadeOutLSFX(int id, float seconds);
	void FadeInBGM(int id, float seconds);
	void FadeOutBGM(int id, float seconds);

	void AddSerializeData(LuaScript* state = nullptr) override;
};
