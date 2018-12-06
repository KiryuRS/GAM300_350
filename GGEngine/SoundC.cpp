/******************************************************************************/
/*!
\File	   	       SoundC.cpp
\Primary Author    Ng Yan Fei 100%
\Project Name      Nameless


Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "stdafx.h"
#include "SoundC.h"
#include <limits>

SoundC::SoundC()
	: audio(COREENGINE_S.GetAudio()), startBGM(false), startLSFX(false),
	  startBGMID(0), startLSFXID(0), fadeDuration(1.0f)
{
}

void SoundC::AddSerializeData(LuaScript* state)
{
	AddSerializable("Start_BGM", startBGM, "", state);
	AddSerializable("Start_LSFX", startLSFX, "", state);
	AddSerializable("Start_BGM_ID", startBGMID, "", state);
	AddSerializable("Start_LSFX_ID", startLSFXID, "", state);
	AddSerializable("Fade_Duration", fadeDuration, "", state);
	AddSerializable("BGMs", bgmSounds, "", state);
	AddSerializable("SFXs", sfxSounds, "", state);
	AddSerializable("LoopingSFXs", lsfxSounds, "", state);
	AddCallableFunction("Stop_BGM", &SoundC::StopBGM, {}, state);
	AddCallableFunction("Play_BGM", &SoundC::PlayBGM, {}, state);
	AddCallableFunction("Play_LSFX", &SoundC::PlayLSFX, {}, state);
	AddCallableFunction("Stop_LSFX", &SoundC::StopLSFX, {}, state);
	AddCallableFunction("Play_SFX", &SoundC::PlaySFX, {}, state);
	AddCallableFunction("FadeIn_BGM", &SoundC::FadeInBGM, {}, state);
	AddCallableFunction("FadeOut_BGM", &SoundC::FadeOutBGM, {}, state);
	AddCallableFunction("FadeIn_LSFX", &SoundC::FadeInLSFX, {}, state);
	AddCallableFunction("FadeOut_LSFX", &SoundC::FadeOutLSFX, {}, state);
}

SoundC::~SoundC()
{
	//COREENGINE_S.GetAudio().Unload();
}


void SoundC::Initialize()
{
	audio.AddNewBGM(bgmSounds);
	audio.AddNewSFX(sfxSounds);
	audio.AddNewLSFX(lsfxSounds);
	if (startBGM && bgmSounds.size())
	{
		audio.PlayBGM(bgmSounds[startBGMID].filename);
	}
	if (startLSFX && lsfxSounds.size())
	{
		audio.PlayLSFX(lsfxSounds[startLSFXID].filename);
	}
}

void SoundC::Update(float)
{
	TransformC *comp_trans = owner->GetComponent<TransformC>();
	if (!comp_trans)
		return;
	// For every 3D audio, we update their position
	// SFX
	for (auto& elem : sfxSounds)
		if (elem.audio_3d)
		{
			AudioFile* aud = const_cast<AudioFile*>(audio.GetAudio(elem.filename, AUDIOTYPE::SFX));
			audio.SetAudioPosition(*aud, comp_trans->GetPosition());
		}
	// LSFX
	for (auto& elem : lsfxSounds)
		if (elem.audio_3d)
		{
			AudioFile* aud = const_cast<AudioFile*>(audio.GetAudio(elem.filename, AUDIOTYPE::LSFX));
			audio.SetAudioPosition(*aud, comp_trans->GetPosition());
		}
	// BGM
	for (auto& elem : bgmSounds)
		if (elem.audio_3d)
		{
			AudioFile* aud = const_cast<AudioFile*>(audio.GetAudio(elem.filename, AUDIOTYPE::BGM));
			audio.SetAudioPosition(*aud, comp_trans->GetPosition());
		}
}

void SoundC::PlaySFX(int id)
{
	if ((unsigned)id < sfxSounds.size())
	{
		audio.PlaySFX(sfxSounds[id].filename);
	}
}

void SoundC::PlayLSFX(int id)
{
	if ((unsigned)id < lsfxSounds.size())
	{
		audio.PlayLSFX(lsfxSounds[id].filename);
	}
}

void SoundC::PlayBGM(int id)
{
	if ((unsigned)id < bgmSounds.size())
		audio.PlayBGM(bgmSounds[id].filename);
}

void SoundC::StopBGM()
{
	audio.StopAllBGM();
}

void SoundC::StopSFX(bool justLSFX)
{
	if (justLSFX)
	{
		audio.StopAllLSFX();
	}
	else
	{
		audio.StopAllSFX();
	}
}

void SoundC::StopLSFX(int id)
{
	if ((unsigned)id < lsfxSounds.size())
	{
		audio.StopAudio(lsfxSounds[id].filename);
	}

}

void SoundC::SetVolume(float sfxVol, float bgmVol)
{
	audio.SetSFXVolume(sfxVol);
	audio.SetLSFXVolume(sfxVol);
	audio.SetBGMVolume(bgmVol);
}

void SoundC::Pause(bool justBGM)
{
	if (justBGM)
	{
		audio.PauseBGM();
	}
	else
	{
		audio.PauseAll();
	}
}

void SoundC::Resume()
{
	audio.ResumeAll();
}

float SoundC::GetSFXVolume(int id)
{
	if (id == -1)
		return audio.GetSFXVolume();
	float vol = audio.GetAudio(sfxSounds[id].filename)->GetVolume();
	return vol;
}

float SoundC::GetBGMVolume(int id)
{
	if (id == -1)
		return audio.GetBGMVolume();
	float vol = audio.GetAudio(bgmSounds[id].filename)->GetVolume();
	return vol;
}

float SoundC::GetLSFXVolume(int id)
{
	if (id == -1)
		return audio.GetLSFXVolume();
	float vol = audio.GetAudio(lsfxSounds[id].filename)->GetVolume();
	return vol;
}


void SoundC::SetSFXVolume(float vol, int soundID)
{
	if (soundID != -1)
		audio.SetSFXVolume(vol);
	else
		const_cast<AudioFile*>(audio.GetAudio(sfxSounds[soundID].filename))->SetVolume(vol);
}

void SoundC::SetLSFXVolume(float vol, int soundID)
{
	if (soundID != -1)
		audio.SetLSFXVolume(vol);
	else
		const_cast<AudioFile*>(audio.GetAudio(lsfxSounds[soundID].filename))->SetVolume(vol);
}

void SoundC::SetBGMVolume(float vol, int soundID)
{
	if (soundID != -1)
		audio.SetBGMVolume(vol);
	else
		const_cast<AudioFile*>(audio.GetAudio(bgmSounds[soundID].filename))->SetVolume(vol);
}

AudioSystem& SoundC::GetAudioSystem()
{
	return audio;
}

void SoundC::FadeInLSFX(int id, float seconds)
{
	auto* file = audio.GetAudio(lsfxSounds[id].filename);
	if (file == nullptr)
	{
		PlayLSFX(id);
	}
	else
	{
		bool playing = false;
		file->channel->isPlaying(&playing);
		if (!playing)
			PlayLSFX(id);
	}
	audio.FadeInAudio(const_cast<AudioFile*>(audio.GetAudio(lsfxSounds[id].filename)), (int)seconds, lsfxSounds[id].volume);
}

void SoundC::FadeOutLSFX(int id, float seconds)
{
	auto* file = audio.GetAudio(lsfxSounds[id].filename);
	if (file == nullptr)
	{
		return;
	}
	audio.FadeOutAudio(const_cast<AudioFile*>(audio.GetAudio(lsfxSounds[id].filename)), (int)seconds, true);
}

void SoundC::FadeInBGM(int id, float seconds)
{
	auto* file = audio.GetAudio(bgmSounds[id].filename);
	if (file == nullptr)
	{
		PlayBGM(id);
	}
	else
	{
		bool playing = false;
		file->channel->isPlaying(&playing);
		if (!playing)
			PlayBGM(id);
	}
	audio.FadeInAudio(const_cast<AudioFile*>(audio.GetAudio(bgmSounds[id].filename)), (int)seconds, bgmSounds[id].volume);
	const_cast<AudioFile*>(audio.GetAudio(bgmSounds[id].filename))->SetMode(FMOD_3D_LINEARSQUAREROLLOFF);
}

void SoundC::FadeOutBGM(int id, float seconds)
{
	auto* file = audio.GetAudio(bgmSounds[id].filename);
	if (file == nullptr)
	{
		return;
	}
	audio.FadeOutAudio(const_cast<AudioFile*>(audio.GetAudio(bgmSounds[id].filename)), (int)seconds, true);
}
