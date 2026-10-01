#pragma once
#include "SDL_mixer.h"
#include "maths.h"
#include "TPinballComponent.h"

// constexpr float MIX_MAX_VOLUME = 128;

struct ChannelInfo
{
	int TimeStamp;
	vector2 Position;
};

class Sound
{
public:
	static std::vector<ChannelInfo> Channels;
	static std::unordered_map<int, MIX_Track*> Tracks;

	static void Init(bool mixOpen, int channels, bool enableFlag, float volume);
	static void Enable(bool enableFlag);
	static void Activate();
	static void Deactivate();
	static void Close();
	static void PlaySound(MIX_Audio* wavePtr, int time, TPinballComponent *soundSource, const char* info);
	// static void PlayMusic(MIX_Audio* wavePtr, int time, TPinballComponent *soundSource, const char* info);
	static MIX_Audio* LoadWaveFile(const std::string& lpName);
	static void FreeSound(MIX_Audio* wave);
	static void SetChannels(int channels);
	static void SetVolume(float volume);
	static void Shutdown();
private:
	static int num_channels;
	static bool enabled_flag;
	static float Volume;
	static bool MixOpen;
	static MIX_Mixer* mixer;
};
