#include "stdafx.h"
#include "AudioSystem.h"

void AudioSystem::StaleAudio(AudioFile & aud_file)
{
	// First we stop the audio from fading (at the current point)
	aud_file.fading = false;
	aud_file.fading_points = 0;
	aud_file.point_dspclock.clear();
	aud_file.point_volume.clear();
	aud_file.stop_aud_fade = false;
	aud_file.fade_time = 0;
	// Remove it from the vec_fading_auds
	auto iter = std::find(vec_fading_auds.begin(), vec_fading_auds.end(), &aud_file);
	if (iter != vec_fading_auds.end())
		vec_fading_auds.erase(iter);
}

void AudioSystem::ClearAllFadingAudios()
{
	for (auto& elem : vec_fading_auds)
	{
		elem->fading = false;
		elem->fading_points = 0;
		elem->point_dspclock.clear();
		elem->point_volume.clear();
		elem->stop_aud_fade = false;
	}

	// Clear everything
	vec_fading_auds.clear();
}

bool AudioSystem::CheckVersion()
{
	unsigned version;
	int num_drivers;
	
	result = system->getVersion(&version);
	FMODErrorCheck(result);
	// Print out our version (Version is stored as 0xaaaabbcc, where "a" is the major version, "b" is the minor version, "c" is the development version
	CONSOLE_SYSTEM("FMOD Version: ", (version == FMOD_VERSION ? "v1.10.8" : "UNKNOWN"));
	if (version < FMOD_VERSION)
	{
		CONSOLE_ERROR("You are using an old version of FMOD! i.e. #", version, ". This program requires ", FMOD_VERSION, " and above.");
		return false;
	}

	// Get the number of sound cards
	result = system->getNumDrivers(&num_drivers);
	FMODErrorCheck(result);
	
	// No Sound Cards (Disable sound)
	if (!num_drivers)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		FMODErrorCheck(result);
	}
	
	return true;
}

AudioFile * AudioSystem::RetrieveAudioFile(FMOD::Sound * sound, FMOD::ChannelGroup* chn_group)
{
	std::map<std::string, AudioFile>* current_map = nullptr;

	if (chn_group == tests.channel_group)
		current_map = &tests.map;
	else if (chn_group == bgm.channel_group)
		current_map = &bgm.map;
	else if (chn_group == sfx.channel_group)
		current_map = &sfx.map;
	else if (chn_group == lsfx.channel_group)
		current_map = &lsfx.map;

	if (current_map)
		for (auto&[str, aud_file] : *current_map)
			if (aud_file.sound == sound)
				return &aud_file;

	return nullptr;
}

AudioFile * AudioSystem::RetrieveAudioFile(const std::string & filename)
{	
	// First, determine if its a relative path or a filename
	std::string alt_path = TOOLS::ReplaceBSlashWithFSlash(filename);
	std::string str = filename;
	// If its a relative path
	if (size_t pos = alt_path.rfind("/"); pos != std::string::npos)
		str = alt_path.substr(pos + 1);
	// Exist in BGM side
	if (auto iter_bgm = bgm.map.find(str); iter_bgm != bgm.map.end())
		return &iter_bgm->second;
	// Exist in SFX side
	if (auto iter_sfx = sfx.map.find(str); iter_sfx != sfx.map.end())
		return &iter_sfx->second;
	// Exist in LSFX side
	if (auto iter_lsfx = lsfx.map.find(str); iter_lsfx != lsfx.map.end())
		return &iter_lsfx->second;

	return nullptr;
}

void AudioInfo::TestSound(unsigned fade_out_sec)
{
	AUDIO_S.TestSound(filename, volume, fade_out_sec);
}

void AudioInfo::StopTestSound()
{
	AUDIO_S.StopTests();
}

AudioSystem::AudioSystem()
	: system{ nullptr }, studio_system{ nullptr }, channels_playing{ 0 },
	  child{ nullptr }, parent{ nullptr }, bgm{ }, sfx{ }, lsfx{ }, tests{ }, playing{ false }, pause_all{ false },
	  audio_rate{ 0 }, dsp_clock{ 0 }, forward_vec{ 0,0,1 }, up_vec{ 0,1,0 }, listener_pos{ 0,0,-1*FMOD_DISTANCEFACTOR },
	  master_group{ nullptr }
{
	// Setup the instance of the studio system engine
	result = FMOD::Studio::System::create(&studio_system);
	FMODErrorCheck(result);

	result = studio_system->getLowLevelSystem(&system);
	FMODErrorCheck(result);

	TOOLS::Assert(CheckVersion(), "ERROR: Wrong FMOD Version! Refer to Log for more information");

	CoreSystem::SetName("Audio");
}

AudioSystem::~AudioSystem()
{
	Unload();

	// Release all channel groups
	result = bgm.channel_group->release();
	FMODErrorCheck(result);
	result = sfx.channel_group->release();
	FMODErrorCheck(result);
	result = lsfx.channel_group->release();
	FMODErrorCheck(result);
	result = tests.channel_group->release();
	FMODErrorCheck(result);
	// Close our system
	result = system->close();
	FMODErrorCheck(result);
	result = studio_system->release();
	FMODErrorCheck(result);

	// Unparent it
	if (parent)
		parent->child = nullptr;
}

AudioSystem * AudioSystem::GetChild()
{
	return child;
}

void AudioSystem::SetChild(AudioSystem * audio)
{
	child = audio;
}

AudioFile * AudioSystem::GetCurrentBGM() const
{
	std::vector<AudioFile*> list_playing;

	// Check through the entire bgm playlist
	AudioFile* playing_bgm = nullptr;
	for (auto& elem : bgm.map)
	{
		bool is_playing = false;
		elem.second.channel->isPlaying(&is_playing);
		if (is_playing)
		{
			list_playing.emplace_back(nullptr);
			list_playing.back() = const_cast<AudioFile*>(&elem.second);
		}
		
	}

	// Check for the number of BGMs playing (as of 08/07/2018, there should only be one BGM playing at a time)
	TOOLS::Assert(list_playing.size() <= 1, "There are more than 1 BGM playing at a time!");
	if (list_playing.size() == 1)
		playing_bgm = list_playing.back();

	return playing_bgm;
}

void AudioSystem::Init(bool secondary)
{
	// Initialize the FMOD engine with 32 channels (and Right Handed System (where +X = Right | +Y = Up | +Z = Backward)
	result = studio_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
	FMODErrorCheck(result);
	// Set the distance units (meters)
	result = system->set3DSettings(1, FMOD_DISTANCEFACTOR, 1);
	FMODErrorCheck(result);
	
	// Create our channels
	result = system->createChannelGroup("BGM Channel", &bgm.channel_group);
	FMODErrorCheck(result);
	result = system->createChannelGroup("SFX Channel", &sfx.channel_group);
	FMODErrorCheck(result);
	result = system->createChannelGroup("LSFX Channel", &lsfx.channel_group);
	FMODErrorCheck(result);
	result = system->createChannelGroup("Test Channel", &tests.channel_group);
	FMODErrorCheck(result);
	// Create the master channel
	result = system->getMasterChannelGroup(&master_group);
	FMODErrorCheck(result);

	// Retrieve the standard audio rate (mixer rate)
	result = system->getSoftwareFormat(&audio_rate, 0, 0);
	FMODErrorCheck(result);

	// Parenting
	if (secondary)
	{
		parent = &COREENGINE_S.GetAudio();
		parent->SetChild(this);
	}

	// Once we are done, we place the BGM, SFX and LSFX channels under the group of master_group
	result = master_group->addGroup(bgm.channel_group);
	FMODErrorCheck(result);
	result = master_group->addGroup(sfx.channel_group);
	FMODErrorCheck(result);
	result = master_group->addGroup(lsfx.channel_group);
	FMODErrorCheck(result);

	// We do not have to cater for Test (as it does not affect the final gameplay)
}

void AudioSystem::Update()
{
	float delta_time = DELTATIME;

	// To determine if the sound is currently fading or have ended (and needs to be removed from the list of vectors)
	bool resume_playing = true;
	for(auto& elem : all_files)
	{
		playing = false;
		// Grab the playing boolean
		elem->channel->isPlaying(&playing);
		// Check if its a channel group that lies in Tests
		FMOD::ChannelGroup* chn_group = nullptr;
		elem->channel->getChannelGroup(&chn_group);
		if (playing && chn_group == tests.channel_group)
			// Then we cannot resume back our BGM, SFX and LSFX
			resume_playing = false;

		if (!playing)
			continue;

		// Update every 3D position of the sounds
		if (elem->three_d_audio)
		{
			// Update our velocity
			FMOD_VECTOR aud_fv_result = MATH::fvAdd(elem->audio_pos, elem->last_pos);
			elem->velocity = MATH::fvDivideValue(aud_fv_result, (1000.f * delta_time));
			// Store our last position for next frame's usage
			elem->last_pos = elem->audio_pos;

			// Update our position
			result = elem->channel->set3DAttributes(&elem->audio_pos, &elem->velocity);
			FMODErrorCheck(result);
		}
	}

	// Attempt to resume back playing (after test playing is done)
	if (resume_playing && pause_all)
	{
		if (bgm.GetPaused())
			ResumeBGM();
		if (sfx.GetPaused())
			ResumeSFX();
		if (lsfx.GetPaused())
			ResumeLSFX();
		pause_all = false;
	}

	std::vector<AudioFile*> to_remove;
	// Checking the vector for any fading actions
	for (auto& elem : vec_fading_auds)
	{
		// by adding into the vector means that the fading boolean is assumed to be true
		if (elem->fading)
		{
			unsigned curr_fadingpoints = 0;
			elem->channel->getFadePoints(&curr_fadingpoints, nullptr, nullptr);
			playing = false;
			elem->channel->isPlaying(&playing);
			if (!curr_fadingpoints)
			{
				// Now we check for the stop aud boolean (or check if the audio is still playing)
				if (((elem->point_volume.empty() || !elem->point_volume.back()) && elem->stop_aud_fade) || !playing)
				{
					elem->channel->stop();
					// Reset the channel by introducing a new channel to the list
					AudioChannelInfo* _parent = nullptr;
					switch (elem->type)
					{
					case AUDIOTYPE::BGM:
						_parent = &bgm;
						break;
					case AUDIOTYPE::LSFX:
						_parent = &lsfx;
						break;
					case AUDIOTYPE::SFX:
						_parent = &sfx;
						break;
					case AUDIOTYPE::TEST:
						_parent = &tests;
						break;
					}
					// Add back to the channel
					result = system->playSound(elem->sound, _parent->channel_group, true, &elem->channel);
					FMODErrorCheck(result);
				}
				elem->fading = false;
				elem->fading_points = 0;
				elem->point_dspclock.clear();
				elem->point_volume.clear();
				elem->fade_time = 0;
				// Add to the to_remove list
				to_remove.emplace_back(nullptr);
				to_remove.back() = elem;
			}
		}
	}

	// Remove all completed fades away
	for (auto& elem : to_remove)
	{
		auto iter = std::find(vec_fading_auds.begin(), vec_fading_auds.end(), elem);
		if (iter != vec_fading_auds.end())
		{
			vec_fading_auds.erase(iter);
			// Check if the audio was from test
			if (elem->type == AUDIOTYPE::TEST)
				// Remove it from the system
				RemoveAudio(elem);
		}
	}

	// Update the listener's velocity and the last position
	FMOD_VECTOR fv_result = MATH::fvAdd(listener_pos, last_pos);
	velocity = MATH::fvDivideValue(fv_result, (1000.f * delta_time));
	last_pos = listener_pos;

	// Update the system's ears
	result = system->set3DListenerAttributes(0, &listener_pos, &velocity, &forward_vec, &up_vec);
	FMODErrorCheck(result);

	// Update our FMOD main component
	result = studio_system->update();
	FMODErrorCheck(result);
	
	// Retrieve the number of channels playing (after all the necessary logic handling)
	result = system->getChannelsPlaying(&channels_playing);
	FMODErrorCheck(result);
}

void AudioSystem::Unload()
{
	// Resume our audio (if necessary)
	ResumeAll();

	StopAllBGM();
	StopAllLSFX();
	StopAllSFX();

	// Release all sounds
	ClearAllFadingAudios();
	ClearAllBGM();
	ClearAllSFX();
	ClearAllLSFX();
	ClearAllTestSounds();
	all_files.clear();
}

bool AudioSystem::IsAudioPlaying(const AudioFile& aud_file) const
{
	// Check if the channel exist
	if (!aud_file.channel)
	{
		CONSOLE_WARNING("There's no channels for this audio: ", aud_file.filename);
		return false;
	}

	bool aud_playing = false;
	aud_file.channel->isPlaying(&aud_playing);
	return aud_playing;
}

void AudioSystem::SetMasterVolume(float vol)
{
	float adjusted_vol = std::max(std::min(vol, 1.f), 0.f);
	master_group->setVolume(adjusted_vol);
}

void AudioSystem::SetBGMVolume(float volume)
{
	float adjusted_vol = std::max(std::min(volume, 1.f), 0.f);
	// Set to the new volume
	result = bgm.channel_group->setVolume(adjusted_vol);
	FMODErrorCheck(result);
}

void AudioSystem::SetSFXVolume(float volume)
{
	float adjusted_vol = std::max(std::min(volume, 1.f), 0.f);
	// Set to the new volume
	result = sfx.channel_group->setVolume(adjusted_vol);
	FMODErrorCheck(result);
}

void AudioSystem::SetLSFXVolume(float volume)
{
	float adjusted_vol = std::max(std::min(volume, 1.f), 0.f);
	// Set to the new volume
	result = lsfx.channel_group->setVolume(adjusted_vol);
	FMODErrorCheck(result);
}

float AudioSystem::GetMasterVolume() const
{
	float volume{ 0 };
	const_cast<AudioSystem*>(this)->result = master_group->getVolume(&volume);
	FMODErrorCheck(result);

	return volume;
}

float AudioSystem::GetBGMVolume() const
{
	float volume{ 0 };
	const_cast<AudioSystem*>(this)->result = bgm.channel_group->getVolume(&volume);
	FMODErrorCheck(result);

	return volume;
}

float AudioSystem::GetSFXVolume() const
{
	float volume{ 0 };
	const_cast<AudioSystem*>(this)->result = sfx.channel_group->getVolume(&volume);
	FMODErrorCheck(result);

	return volume;
}

float AudioSystem::GetLSFXVolume() const
{
	float volume{ 0 };
	const_cast<AudioSystem*>(this)->result = lsfx.channel_group->getVolume(&volume);
	FMODErrorCheck(result);

	return volume;
}

void AudioSystem::SetListenerPosition(const FMOD_VECTOR & vec3)
{
	listener_pos = vec3;
}

bool AudioSystem::SetAudioPosition(const std::string & filename, AUDIOTYPE type, const FMOD_VECTOR & vec3)
{
	bool success = false;
	std::map<std::string, AudioFile>::iterator iter;
	AudioChannelInfo* category = nullptr;

	switch (type)
	{
	case AUDIOTYPE::BGM:
		category = &bgm;
		break;

	case AUDIOTYPE::SFX:
		category = &sfx;
		break;

	case AUDIOTYPE::LSFX:
		category = &lsfx;
		break;

	default:
		// We do not cater for Test since its only going to be a once only audio play
		break;
	}

	if (category)
	{
		// Attempt to search for the filename, and then pass it into the function
		iter = category->map.find(filename);
		if (iter != category->map.end())
		{
			iter->second.audio_pos = vec3;
			success = true;
		}
	}

	return success;
}

bool AudioSystem::SetAudioPosition(AudioFile & audio_file, const FMOD_VECTOR & vec3)
{
	// Updates the positional vector of the audiofile
	audio_file.audio_pos = vec3;
	return true;
}

bool AudioSystem::CheckFileSupported(const std::string & filename)
{
	// Checks if the audio file is supported by (both engine and) FMOD
	size_t support1 = filename.rfind(".mp3"), support2 = filename.rfind(".wav"), support3 = filename.rfind(".ogg");
	if (support1 != std::string::npos || support2 != std::string::npos || support3 != std::string::npos)
		return true;

	return false;
}

void AudioSystem::ReverseAudio(const std::string & filename)
{
	// Search for the audio in the respective categories
	auto audiofile = const_cast<AudioFile*>(GetAudio(filename));
	if (!audiofile)
	{
		CONSOLE_ERROR("Unable to reverse the audio as the filename was not found!");
		return;
	}

	ReverseAudio(*audiofile);
}

void AudioSystem::ReverseAudio(AudioFile & audio_file)
{
	// Retrieve the frequency value
	float curr_freq{ 0 };
	result = audio_file.channel->getFrequency(&curr_freq);
	FMODErrorCheck(result);
	// Set a new frequency (by negative it)
	result = audio_file.channel->setFrequency(curr_freq * -1);
	FMODErrorCheck(result);
}

AudioFile* AudioSystem::TestSound(const std::string &file, float volume, unsigned fade_out_sec, bool _pause_all)
{
#ifdef _DEBUG
	if (!CheckFileSupported(file))
	{
		CONSOLE_ERROR("Audio file is not supported by the system! The given filename is: ", file);
		return nullptr;
	}
#endif

	if (_pause_all)
	{
		PauseBGM();
		PauseSFX();
		PauseLSFX();
		pause_all = true;
	}

	std::string str = TOOLS::ReplaceBSlashWithFSlash(file);
	size_t pos = str.rfind("/");
	// Check if there exist the relative path
	if (pos != std::string::npos)
		str = str.substr(pos + 1);
	auto iter = tests.map.find(str);
	// Check if it alredy exist inside the map
	if (iter != tests.map.end())
	{
		AudioFile& aud = iter->second;
		// Since we have played before, we will stop the audio from playing and then "reset it again"
		aud.channel->stop();
		result = system->playSound(aud.sound, tests.channel_group, false, &aud.channel);
		FMODErrorCheck(result);
		aud.channel->setVolume(volume);
		// Check if the audio was previously fading
		if (aud.fading)
		{
			StaleAudio(aud);
			// Apply the fading if there is
			if (fade_out_sec)
				FadeOutAudio(&aud, fade_out_sec, true);
		}
		return &aud;
	}

	// Create a new test sound
	tests.sounds.emplace_back(nullptr);
	result = system->createSound(file.c_str(), FMOD_2D, 0, &(tests.sounds.back()));
	FMODErrorCheck(result);
	auto& new_test_file = tests.map[str];
	new_test_file.filename = str;
	new_test_file.relative_path = file;
	new_test_file.sound = tests.sounds.back();
	new_test_file.three_d_audio = false;
	new_test_file.type = AUDIOTYPE::TEST;
	// Start to play the sound
	result = system->playSound(new_test_file.sound, tests.channel_group, false, &new_test_file.channel);
	FMODErrorCheck(result);
	new_test_file.channel->setVolume(volume);
	if (fade_out_sec)
		FadeOutAudio(&new_test_file, fade_out_sec, true);
	all_files.emplace_back();
	all_files.back() = &new_test_file;
	return &new_test_file;
}

void AudioSystem::StopTestSound(const AudioFile * aud_file)
{
	if (pause_all)
	{
		// Unpause all audios
		ResumeAll();
		pause_all = false;
	}

	// Remove the test sound from the test section and from the channel
	auto iter = std::find(all_files.begin(), all_files.end(), aud_file);
	if (iter == all_files.end())
	{
		CONSOLE_WARNING("Audio File not found in Audio System! Unable to stop the audio");
		return;
	}
	// Stop the audio file
	aud_file->channel->stop();
	aud_file->sound->release();
	tests.map.erase(aud_file->filename);
	auto iter_s = std::find(tests.sounds.begin(), tests.sounds.end(), aud_file->sound);
	tests.sounds.erase(iter_s);
}

const AudioFile * AudioSystem::GetAudio(const std::string &filename, AUDIOTYPE type) const
{
	// If we already know the type, just get it straight from the files
	if (type != AUDIOTYPE::NONE)
	{
		// First, determine if its a relative path or a filename
		std::string alt_path = TOOLS::ReplaceBSlashWithFSlash(filename);
		std::string str = filename;
		// If its a relative path
		if (size_t pos = alt_path.rfind("/"); pos != std::string::npos)
			str = alt_path.substr(pos + 1);
		switch (type)
		{
		case AUDIOTYPE::BGM:
			if (auto iter = bgm.map.find(str); iter != bgm.map.end())
				return &iter->second;
			break;
		case AUDIOTYPE::SFX:
			if (auto iter = sfx.map.find(str); iter != sfx.map.end())
				return &iter->second;
			break;
		case AUDIOTYPE::LSFX:
			if (auto iter = lsfx.map.find(str); iter != lsfx.map.end())
				return &iter->second;
			break;
		// Not handling it for Test Sounds
		}
	}
	// Use the private function to help since it already exist
	return const_cast<AudioSystem * const>(this)->RetrieveAudioFile(filename);
}

bool AudioSystem::StoreAllAudio(const std::string& relative_path)
{
	std::vector<AudioInfo> bgm_files, sfx_files, lsfx_files;

#if FULLC17
	for (auto& path : fs::recursive_directory_iterator(relative_path))
	{
		// First, convert the path's slashes to the correct one
		std::string alt_path = TOOLS::ReplaceBSlashWithFSlash(path.path().string());
		std::string audiofile = alt_path.substr(alt_path.rfind("/") + 1);
		// Check if its a supported file
		if (!CheckFileSupported(audiofile))
			continue;

		// Determine for 2D audio or 3D audio
		bool audio_3d = alt_path.rfind("/3D") != std::string::npos;

		// Finding for all BGM files
		size_t bgm_pos = alt_path.find("BGM");
		if (bgm_pos != std::string::npos)
		{
			bgm_files.emplace_back(alt_path, 1.f, audio_3d);
			continue;
		}

		// Finding for all SFX files
		size_t sfx_pos = alt_path.find("SFX");
		if (sfx_pos != std::string::npos)
		{
			sfx_files.emplace_back(alt_path, 1.f, audio_3d);
			continue;
		}

		// Finding for all LSFX files
		size_t lsfx_pos = alt_path.find("LSFX");
		if (lsfx_pos != std::string::npos)
			lsfx_files.emplace_back(alt_path, 1.f, audio_3d);
	}

#ifdef _DEBUG
	if (bgm_files.empty() && sfx_files.empty() && lsfx_files.empty())
		CONSOLE_WARNING("There are no audio files in the specified folder!");
	else
#endif
	{
		AddNewBGM(bgm_files);
		AddNewSFX(sfx_files);
		AddNewLSFX(lsfx_files);
	}

	return true;
#else

	// @todo: Currently the version that was implemented above only works for C++17 and above
	CONSOLE_ERROR("Please upgrade the MSC Version to at least 19.14 to use this function!");
	return false;
#endif
}

bool AudioSystem::SetAudioVolume(const std::string & filename, AUDIOTYPE type, float volume)
{
	bool success = false;
	std::map<std::string, AudioFile>::iterator iter;
	AudioChannelInfo* category = nullptr;

	std::string str = TOOLS::ReplaceBSlashWithFSlash(filename);
	size_t pos = str.rfind("/");
	// Check if there exist the relative path
	if (pos != std::string::npos)
		str = str.substr(pos + 1);

	switch (type)
	{
	case AUDIOTYPE::BGM:
		category = &bgm;
		break;

	case AUDIOTYPE::SFX:
		category = &sfx;
		break;

	case AUDIOTYPE::LSFX:
		category = &lsfx;
		break;

	default:
		// We do not cater for Test since its only going to be a once only audio play
		break;
	}

	if (category)
	{
		// Attempt to search for the filename, and then pass it into the function
		iter = category->map.find(str);
		if (iter != category->map.end())
			success = SetAudioVolume(iter->second, volume);
	}

	return success;
}

bool AudioSystem::SetAudioVolume(AudioFile & audio_file, float volume)
{	
	// Updates the channel as well (in any case if its playing)
	audio_file.channel->setVolume(volume);
	return true;
}

void AudioSystem::PauseBGM(bool)
{
	// Only pause if the BGM side is unpaused
	if (!bgm.GetPaused())
	{
		result = bgm.channel_group->setPaused(true);
		FMODErrorCheck(result);
	}
}

void AudioSystem::PauseSFX()
{
	// Only pause if the SFX side is unpaused
	if (!sfx.GetPaused())
	{
		result = sfx.channel_group->setPaused(true);
		FMODErrorCheck(result);
	}
}

void AudioSystem::PauseLSFX()
{
	// Only pause if the LSFX side is unpaused
	if (!lsfx.GetPaused())
	{
		result = lsfx.channel_group->setPaused(true);
		FMODErrorCheck(result);
	}
}

void AudioSystem::PauseTests()
{
	// Only pause if the Tests side is unpaused
	if (!tests.GetPaused())
	{
		result = tests.channel_group->setPaused(true);
		FMODErrorCheck(result);
	}
}

void AudioSystem::PauseAll()
{
	PauseLSFX();
	PauseSFX();
	PauseBGM();
	PauseTests();
}

bool AudioSystem::PauseAudio(const std::string & audio_filename)
{
	AudioFile* audio_file = RetrieveAudioFile(audio_filename);
	if (!audio_file)
		return false;

	if (!audio_file->GetPaused())
	{
		audio_file->channel->setPaused(true);
	}

	return true;
}

bool AudioSystem::PauseAudio(AudioFile & aud_file)
{
	if (!aud_file.GetPaused())
		aud_file.channel->setPaused(true);
	return true;
}

void AudioSystem::ResumeBGM(bool)
{
	if (bgm.GetPaused())
	{
		result = bgm.channel_group->setPaused(false);
		FMODErrorCheck(result);
	}
}

void AudioSystem::ResumeSFX()
{
	if (sfx.GetPaused())
	{
		result = sfx.channel_group->setPaused(false);
		FMODErrorCheck(result);
	}
}

void AudioSystem::ResumeLSFX()
{
	if (lsfx.GetPaused())
	{
		result = lsfx.channel_group->setPaused(false);
		FMODErrorCheck(result);
	}
}

void AudioSystem::ResumeTests()
{
	if (tests.GetPaused())
	{
		result = tests.channel_group->setPaused(false);
		FMODErrorCheck(result);
	}
}

void AudioSystem::ResumeAll()
{
	ResumeLSFX();
	ResumeSFX();
	ResumeBGM();
	ResumeTests();
}

bool AudioSystem::ResumeAudio(const std::string & audio_filename)
{
	AudioFile* audio_file = RetrieveAudioFile(audio_filename);
	// Check if the file exist
	if (!audio_file)
		return false;

	// Sanity check for channels
	if (!audio_file->channel)
	{
		CONSOLE_WARNING("There's no channels for this audio: ", audio_file->filename);
		return false;
	}

	// Resume the audio file
	if (audio_file->GetPaused())
		audio_file->channel->setPaused(false);

	return true;
}

bool AudioSystem::ResumeAudio(AudioFile & aud_file)
{
	// Sanity check for channels
	if (!aud_file.channel)
	{
		CONSOLE_WARNING("There's no channels for this audio: ", aud_file.filename);
		return false;
	}

	// Resume the audio file
	if (aud_file.GetPaused())
		aud_file.channel->setPaused(false);

	return true;
}

AudioFile* AudioSystem::PlaySFX(const std::string &filename)
{
	// First, determine if its a relative path or a filename
	std::string alt_path = TOOLS::ReplaceBSlashWithFSlash(filename);
	std::string str = filename;
	// If its a relative path
	if (size_t pos = alt_path.rfind("/"); pos != std::string::npos)
		str = alt_path.substr(pos + 1);
	if (sfx.map.find(str) != sfx.map.end())
	{
		auto& aud = sfx.map[str];
#ifdef _DEBUG
		// Compare the relative path to see if it matches
		if (filename != aud.relative_path)
			CONSOLE_WARNING("A similar version of the audio file is found but has incorrect path name!\n\t\t\t\t\t\t The pathname given was: ", filename);
#endif
		// Check if the audio was previously fading, if it is, we stale it
		if (aud.fading)
			StaleAudio(aud);
		result = system->playSound(aud.sound, sfx.channel_group, false, &aud.channel);
		FMODErrorCheck(result);
		result = aud.channel->setVolume(aud.volume);
		FMODErrorCheck(result);
		if (aud.three_d_audio)
		{
			result = aud.channel->set3DAttributes(&aud.audio_pos, &aud.velocity);
			FMODErrorCheck(result);
		}
		all_files.emplace_back();
		all_files.back() = &aud;
		return &aud;
	}

#ifdef _DEBUG
	CONSOLE_ERROR("SFX Audio Not Found! Please ensure that you have keyed in the correct filename or the pathname");
#endif
	return nullptr;
}

AudioFile* AudioSystem::PlayLSFX(const std::string &filename)
{
	// First, determine if its a relative path or a filename
	std::string alt_path = TOOLS::ReplaceBSlashWithFSlash(filename);
	std::string str = filename;
	// If its a relative path
	if (size_t pos = alt_path.rfind("/"); pos != std::string::npos)
		str = alt_path.substr(pos + 1);
	if (lsfx.map.find(str) != lsfx.map.end())
	{
		auto& aud = lsfx.map[str];
#ifdef _DEBUG
		// Compare the relative path to see if it matches
		if (filename != aud.relative_path)
			CONSOLE_WARNING("A similar version of the audio file is found but has incorrect path name!\n\t\t\t\t\t\t The pathname given was: ", filename);
#endif
		// Check if the audio was previously fading, if it is, we stale it
		if (aud.fading)
			StaleAudio(aud);
		result = system->playSound(aud.sound, lsfx.channel_group, false, &aud.channel);
		FMODErrorCheck(result);
		result = aud.channel->setVolume(aud.volume);
		FMODErrorCheck(result);
		if (aud.three_d_audio)
		{
			result = aud.channel->set3DAttributes(&aud.audio_pos, &aud.velocity);
			FMODErrorCheck(result);
		}
		all_files.emplace_back();
		all_files.back() = &aud;
		return &aud;
	}

#ifdef _DEBUG
	CONSOLE_ERROR("LSFX Audio Not Found! Please ensure that you have keyed in the correct filename or the pathname");
#endif

	return nullptr;
}

AudioFile* AudioSystem::PlayBGM(const std::string &filename)
{
	// First, determine if its a relative path or a filename
	std::string alt_path = TOOLS::ReplaceBSlashWithFSlash(filename);
	std::string str = filename;
	// If its a relative path
	if (size_t pos = alt_path.rfind("/"); pos != std::string::npos)
		str = alt_path.substr(pos + 1);
	if (bgm.map.find(str) != bgm.map.end())
	{
		auto& aud = bgm.map[str];
#ifdef _DEBUG
		// Compare the relative path to see if it matches
		if (filename != aud.relative_path)
			CONSOLE_WARNING("A similar version of the audio file is found but has incorrect path name!\n\t\t\t\t\t\t The pathname given was: ", filename);
#endif
		// Check if the audio was previously fading, if it is, we stale it
		if (aud.fading)
			StaleAudio(aud);
		result = system->playSound(aud.sound, bgm.channel_group, false, &aud.channel);
		FMODErrorCheck(result);
		result = aud.channel->setVolume(aud.volume);
		FMODErrorCheck(result);
		if (aud.three_d_audio)
		{
			result = aud.channel->set3DAttributes(&aud.audio_pos, &aud.velocity);
			FMODErrorCheck(result);
			result = aud.channel->set3DSpread(aud.spread);
			FMODErrorCheck(result);
		}
		all_files.emplace_back();
		all_files.back() = &aud;
		return &aud;
	}

#ifdef _DEBUG
	CONSOLE_ERROR("BGM Audio Not Found! Please ensure that you have keyed in the correct filename or pathname");
#endif

	return nullptr;
}

void AudioSystem::StopAllSFX()
{
	result = sfx.channel_group->stop();
	FMODErrorCheck(result);
}

void AudioSystem::StopAllLSFX()
{
	result = lsfx.channel_group->stop();
	FMODErrorCheck(result);
}

void AudioSystem::StopAllBGM()
{
	result = bgm.channel_group->stop();
	FMODErrorCheck(result);
}

void AudioSystem::StopTests()
{
	// Remove every single audio file from the data
	ClearAllTestSounds();

	// Stop all of the channel_group
	result = tests.channel_group->stop();
	FMODErrorCheck(result);
}

void AudioSystem::StopAudio(const std::string & filename)
{
	// First, determine if the filename is in the system
	auto audio = const_cast<AudioFile*>(GetAudio(filename));
	if (!audio)
	{
		CONSOLE_ERROR("Unable to stop audio as audiofile is not found!");
		return;
	}

	// Sanity Check if the channel has already stopped playing
	if (!audio->channel)
		return;

	// Stop the current audio
	result = audio->channel->stop();
	FMODErrorCheck(result);
}

void AudioSystem::StopAudio(AudioFile & audio_file)
{
	// Sanity Check if the channel has already stopped playing
	if (!audio_file.channel)
		return;

	// Stop the current audio
	result = audio_file.channel->stop();
	FMODErrorCheck(result);
}

void AudioSystem::ClearAllSFX()
{
	// Firstly to remove them from the channels section
	for (auto&[str, aud_file] : sfx.map)
	{
		auto iter = std::find(all_files.begin(), all_files.end(), &aud_file);
		if (iter != all_files.end())
			all_files.erase(iter);
		aud_file.channel->stop();
	}

	for (auto& elem : sfx.sounds)
		elem->release();
	sfx.sounds.clear();

	// Clear the map as well
	sfx.map.clear();
}

void AudioSystem::ClearAllLSFX()
{
	// Firstly to remove them from the channels section
	for (auto&[str, aud_file] : lsfx.map)
	{
		auto iter = std::find(all_files.begin(), all_files.end(), &aud_file);
		if (iter != all_files.end())
			all_files.erase(iter);
		aud_file.channel->stop();
	}

	for (auto& elem : lsfx.sounds)
		elem->release();
	lsfx.sounds.clear();

	// Clear the map as well
	lsfx.map.clear();
}

void AudioSystem::ClearAllBGM()
{
	// Firstly to remove them from the channels section
	for (auto&[str, aud_file] : bgm.map)
	{
		auto iter = std::find(all_files.begin(), all_files.end(), &aud_file);
		if (iter != all_files.end())
			all_files.erase(iter);
		aud_file.channel->stop();
	}

	for (auto& elem : bgm.sounds)
		elem->release();
	bgm.sounds.clear();

	// Clear the map as well
	bgm.map.clear();
}

void AudioSystem::ClearAllTestSounds()
{
	// Firstly to remove them from the channels section
	for (auto&[str, aud_file] : tests.map)
	{
		auto iter = std::find(all_files.begin(), all_files.end(), &aud_file);
		if (iter != all_files.end())
			all_files.erase(iter);
		aud_file.channel->stop();
	}

	for (auto& elem : tests.sounds)
		elem->release();
	tests.sounds.clear();
	// Clear the map as well
	tests.map.clear();
}

void AudioSystem::AddNewSFX(const std::vector<AudioInfo>& vec)
{
	// Add every single filename from the list to our sfx
	for (auto& elem : vec)
	{
		// Checks if the audio passed in is supported
		if (!CheckFileSupported(elem.filename))
		{
			CONSOLE_ERROR("Audio file is not supported by the system! The given filename is: ", elem.filename);
			continue;
		}

		// Extract out the string that doesn't contain the relative path
		std::string str = TOOLS::ReplaceBSlashWithFSlash(elem.filename);
		size_t pos = str.rfind("/");
		// Check if there exist the relative path
		if (pos != std::string::npos)
			str = str.substr(pos + 1);

		// Check if the current sound already exist
		auto iter = sfx.map.find(str);
		if (iter != sfx.map.end())
			// Skip adding to the list
			continue;

		sfx.sounds.emplace_back(nullptr);
		if (elem.audio_3d)
		{
			result = system->createSound(elem.filename.c_str(), FMOD_3D, 0, &(sfx.sounds.back()));
			FMODErrorCheck(result);
			result = sfx.sounds.back()->set3DMinMaxDistance(FMOD_MIN_DISTANCE * FMOD_DISTANCEFACTOR, FMOD_MAX_DISTANCE * FMOD_DISTANCEFACTOR);
			FMODErrorCheck(result);
		}
		else
		{
			result = system->createSound(elem.filename.c_str(), FMOD_DEFAULT, 0, &(sfx.sounds.back()));
			FMODErrorCheck(result);
		}
		auto& new_audio = sfx.map[str];
		new_audio.filename = str;
		new_audio.relative_path = elem.filename;
		new_audio.sound = sfx.sounds.back();
		new_audio.volume = elem.volume;
		new_audio.type = AUDIOTYPE::SFX;
		new_audio.three_d_audio = elem.audio_3d;
		new_audio.audio_pos = elem.pos;
		new_audio.spread = elem.deg_spread;
	}
}

void AudioSystem::AddNewLSFX(const std::vector<AudioInfo>& vec)
{
	// Add every single filename from the list to our sfx
	for (auto& elem : vec)
	{
#ifdef _DEBUG
		// Checks if the audio passed in is supported
		if (!CheckFileSupported(elem.filename))
		{
			CONSOLE_ERROR("Audio file is not supported by the system! The given filename is: ", elem.filename);
			continue;
		}
#endif

		// Extract out the string that doesn't contain the relative path
		std::string str = TOOLS::ReplaceBSlashWithFSlash(elem.filename);
		size_t pos = str.rfind("/");
		// Check if there exist the relative path
		if (pos != std::string::npos)
			str = str.substr(pos + 1);

		// Check if the current sound already exist
		auto iter = lsfx.map.find(str);
		if (iter != lsfx.map.end())
			// Skip adding to the list
			continue;

		lsfx.sounds.emplace_back(nullptr);
		if (elem.audio_3d)
		{
			result = system->createSound(elem.filename.c_str(), FMOD_3D, 0, &(lsfx.sounds.back()));
			FMODErrorCheck(result);
			result = lsfx.sounds.back()->set3DMinMaxDistance(FMOD_MIN_DISTANCE * FMOD_DISTANCEFACTOR, FMOD_MAX_DISTANCE * FMOD_DISTANCEFACTOR);
			FMODErrorCheck(result);
			result = lsfx.sounds.back()->setMode(FMOD_LOOP_NORMAL);
			FMODErrorCheck(result);
		}
		else
		{
			result = system->createSound(elem.filename.c_str(), FMOD_LOOP_NORMAL, 0, &(lsfx.sounds.back()));
			FMODErrorCheck(result);
		}
		auto& new_audio = lsfx.map[str];
		new_audio.filename = str;
		new_audio.relative_path = elem.filename;
		new_audio.sound = lsfx.sounds.back();
		new_audio.volume = elem.volume;
		new_audio.type = AUDIOTYPE::LSFX;
		new_audio.three_d_audio = elem.audio_3d;
		new_audio.audio_pos = elem.pos;
		new_audio.spread = elem.deg_spread;
	}
}

void AudioSystem::AddNewBGM(const std::vector<AudioInfo>& vec)
{
	// Add every single filename from the list to our bgm
	for (auto& elem : vec)
	{
#ifdef _DEBUG
		// Checks if the audio passed in is supported
		if (!CheckFileSupported(elem.filename))
		{
			CONSOLE_ERROR("Audio file is not supported by the system! The given filename is: ", elem.filename);
			continue;
		}
#endif

		// Extract out the string that doesn't contain the relative path
		std::string str = TOOLS::ReplaceBSlashWithFSlash(elem.filename);
		size_t pos = str.rfind("/");
		// Check if there exist the relative path
		if (pos != std::string::npos)
			str = str.substr(pos + 1);

		// Check if the current sound already exist
		auto iter = bgm.map.find(str);
		if (iter != bgm.map.end())
			// Skip adding to the list
			continue;

		bgm.sounds.emplace_back(nullptr);
		// Determine the audio dimension
		if (elem.audio_3d)
		{
			result = system->createSound(elem.filename.c_str(), FMOD_3D, 0, &(bgm.sounds.back()));
			FMODErrorCheck(result);
			result = bgm.sounds.back()->set3DMinMaxDistance(FMOD_MIN_DISTANCE * FMOD_DISTANCEFACTOR, FMOD_MAX_DISTANCE * FMOD_DISTANCEFACTOR);
			FMODErrorCheck(result);
			// Set our mode to looping normal (since we cant do that in the createSound anymore [replaced by 3D])
			result = bgm.sounds.back()->setMode(FMOD_LOOP_NORMAL);
			FMODErrorCheck(result);
		}
		else
		{
			result = system->createSound(elem.filename.c_str(), FMOD_LOOP_NORMAL, 0, &(bgm.sounds.back()));
			FMODErrorCheck(result);
		}
		auto& new_audio = bgm.map[str];
		new_audio.filename = str;
		new_audio.relative_path = elem.filename;
		new_audio.sound = bgm.sounds.back();
		new_audio.volume = elem.volume;
		new_audio.type = AUDIOTYPE::BGM;
		new_audio.three_d_audio = elem.audio_3d;
		new_audio.audio_pos = elem.pos;
		new_audio.spread = elem.deg_spread;
	}
}

void AudioSystem::AddNewBGM(const AudioInfo& audio_info)
{
	AddNewBGM(std::vector<AudioInfo>{ audio_info });
}

void AudioSystem::AddNewSFX(const AudioInfo& audio_info)
{
	AddNewSFX(std::vector<AudioInfo>{ audio_info });
}

void AudioSystem::AddNewLSFX(const AudioInfo& audio_info)
{
	AddNewLSFX(std::vector<AudioInfo>{ audio_info });
}

void AudioSystem::RemoveAudio(const AudioFile * aud_file)
{
	// Stop it from playing if it is
	aud_file->channel->stop();

	// Next, determine which of the sub channel_group does it belong to and remove it
	switch (aud_file->type)
	{
	case AUDIOTYPE::BGM:
		bgm.map.erase(aud_file->filename);
		{
			auto _iter = std::find(bgm.sounds.begin(), bgm.sounds.end(), aud_file->sound);
			if (_iter != bgm.sounds.end())
				bgm.sounds.erase(_iter);
		}
		break;

	case AUDIOTYPE::SFX:
		sfx.map.erase(aud_file->filename);
		{
			auto _iter = std::find(sfx.sounds.begin(), sfx.sounds.end(), aud_file->sound);
			if (_iter != sfx.sounds.end())
				sfx.sounds.erase(_iter);
		}
		break;

	case AUDIOTYPE::LSFX:
		lsfx.map.erase(aud_file->filename);
		{
			auto _iter = std::find(lsfx.sounds.begin(), lsfx.sounds.end(), aud_file->sound);
			if (_iter != lsfx.sounds.end())
				lsfx.sounds.erase(_iter);
		}
		break;

	case AUDIOTYPE::TEST:
		tests.map.erase(aud_file->filename);
		{
			auto _iter = std::find(tests.sounds.begin(), tests.sounds.end(), aud_file->sound);
			if (_iter != tests.sounds.end())
				tests.sounds.erase(_iter);
		}
		break;
	}

	// Stop fading (if there is)
	if (aud_file->fading)
		StaleAudio(*const_cast<AudioFile*>(aud_file));
	// Release the sound
	aud_file->sound->release();
	// Search through the all_files and remove it from the list
	auto iter = std::find(all_files.begin(), all_files.end(), aud_file);
	if (iter != all_files.end())
		all_files.erase(iter);
}

bool AudioSystem::FadeInAudio(AudioFile* aud_file, unsigned seconds, float volume)
{
	// Sanity check if the fading is ongoing
	if (aud_file->fading)
		StaleAudio(*aud_file);

	result = aud_file->channel->getDSPClock(nullptr, &dsp_clock);
	FMODErrorCheck(result);
	aud_file->channel->addFadePoint(dsp_clock, 0);
	aud_file->channel->addFadePoint(dsp_clock + audio_rate * seconds, volume);
	// Modify the values
	aud_file->fading = true;
	aud_file->fade_time = seconds;
	// Add it to our vector
	vec_fading_auds.emplace_back(nullptr);
	vec_fading_auds.back() = aud_file;

	// Grab the fading points
	result = aud_file->channel->getFadePoints(&aud_file->fading_points, nullptr, nullptr);
	FMODErrorCheck(result);
	// Assign the vectors
	aud_file->point_volume.resize(aud_file->fading_points);
	aud_file->point_dspclock.resize(aud_file->fading_points);
	aud_file->channel->getFadePoints(nullptr, &aud_file->point_dspclock[0], &aud_file->point_volume[0]);

	return true;
}

bool AudioSystem::FadeOutAudio(AudioFile* aud_file, unsigned seconds, bool stop_aud)
{
	// Sanity check if the fading is ongoing
	if (aud_file->fading)
		StaleAudio(*aud_file);

	// Get the current volume
	float curr_volume = aud_file->GetVolume();
	result = aud_file->channel->getDSPClock(nullptr, &dsp_clock);
	FMODErrorCheck(result);
	result = aud_file->channel->addFadePoint(dsp_clock, curr_volume);
	FMODErrorCheck(result);
	result = aud_file->channel->addFadePoint(dsp_clock + audio_rate * seconds, 0);
	FMODErrorCheck(result);
	// Check for the stop_aud boolean to determine if we need to stop the audio after we finished fading
	aud_file->stop_aud_fade = stop_aud;
	aud_file->fading = true;
	aud_file->fade_time = seconds;
	// Add it to our vector
	vec_fading_auds.emplace_back(nullptr);
	vec_fading_auds.back() = aud_file;

	// Grab the fading points
	result = aud_file->channel->getFadePoints(&aud_file->fading_points, nullptr, nullptr);
	FMODErrorCheck(result);
	// Assign the vectors
	aud_file->point_volume.resize(aud_file->fading_points);
	aud_file->point_dspclock.resize(aud_file->fading_points);
	aud_file->channel->getFadePoints(nullptr, &aud_file->point_dspclock[0], &aud_file->point_volume[0]);

	return true;
}
