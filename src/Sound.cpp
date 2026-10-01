#include "SDL_mixer.h"
#include "pch.h"
#include "options.h"
#include "Sound.h"
#include "maths.h"

int Sound::num_channels;
bool Sound::enabled_flag = false;
std::vector<ChannelInfo> Sound::Channels{};
std::unordered_map<int, MIX_Track*> Sound::Tracks{};
float Sound::Volume = 1.0f;
bool Sound::MixOpen = false;
MIX_Mixer* Sound::mixer = nullptr;

void Sound::Init(bool mixOpen, int channels, bool enableFlag, float volume)
{
	MixOpen = mixOpen;
	Volume = volume;
	SetChannels(channels);
	Enable(enableFlag);
}

void Sound::Enable(bool enableFlag)
{
	enabled_flag = enableFlag;
	if (MixOpen && !enableFlag)
	{
		MIX_StopAllTracks(mixer, 0);
	}
}

void Sound::Activate()
{
	if (MixOpen)
	{
		MIX_ResumeAllTracks(mixer);
	}
}

void Sound::Deactivate()
{
	if (MixOpen)
	{
		MIX_PauseAllTracks(mixer);
	}
}

void Sound::Close()
{
	Enable(false);
	Channels.clear();
}

// keep in mind that track 0 is for music
void Sound::PlaySound(MIX_Audio* wavePtr, int time, TPinballComponent* soundSource, const char* info)
{
	if (MixOpen && wavePtr && enabled_flag)
	{
		// Situation when all channels are busy
		int tracks_playing = 1;
		for (size_t i = 1; i < Tracks.size(); ++i)
		{
			if (MIX_TrackPlaying(Tracks[i]))
			{
				tracks_playing++;
			} else {
				break;
			}
		}
		if (tracks_playing == num_channels)
		{
			auto cmp = [](const ChannelInfo& a, const ChannelInfo& b)
			{
				return a.TimeStamp < b.TimeStamp;
			};
			auto min = std::min_element(Channels.begin() + 1, Channels.end(), cmp);
			auto oldestChannel = static_cast<int>(std::distance(Channels.begin(), min));
			MIX_StopTrack(Tracks[oldestChannel], 0);
		}

		// auto channel = MIX_PlayAudio(mixer, wavePtr);
		int channel = tracks_playing;
		MIX_SetTrackAudio(Tracks[channel], wavePtr);
		if (channel != -1)
		{
			Channels[channel].TimeStamp = time;
			if (options::Options.SoundStereo)
			{
				// Positional audio uses collision grid 2D coordinates normalized to [0, 1]
				// Point (0, 0) is bottom left table corner; point (1, 1) is top right table corner.
				// Z is defined as: 0 at table level, positive axis goes up from table surface.

				// Get the source sound position.
				// Sound without position are assumed to be at the center top of the table.
				vector3 soundPos{};
				if (soundSource)
				{
					auto soundPos2D = soundSource->get_coordinates();
					soundPos = {soundPos2D.X, soundPos2D.Y, 0.0f};
				}
				else
				{
					soundPos = {0.5f, 1.0f, 0.0f};
				}
				Channels[channel].Position = soundPos;

				// Listener is positioned at the bottom center of the table,
				// at 0.5 height, so roughly a table half - length.
				// vector3 playerPos = {0.5f, 0.0f, 0.5f};
				// auto soundDir = maths::vector_sub(soundPos, playerPos);

				// Find sound angle from positive Y axis in clockwise direction with atan2
				// Remap atan2 output from (-Pi, Pi] to [0, 2 * Pi)
				// auto angle = fmodf(atan2(soundDir.X, soundDir.Y) + Pi * 2, Pi * 2);
				// auto angleDeg = angle * 180.0f / Pi;
				// auto angleSdl = static_cast<Sint16>(angleDeg);

				// Distance from listener to the sound position is roughly in the [0, ~1.22] range.
				// Remap to [0, 122] by multiplying by 100 and cast to an integer.
				// auto distance = static_cast<Uint8>(100.0f * maths::magnitude(soundDir));

				// Mix_SetPosition expects an angle in (Sint16)degrees, where
				// angle 0 is due north, and rotates clockwise as the value increases.
				// Mix_SetPosition expects a (Uint8)distance from 0 (near) to 255 (far).
				MIX_Point3D position = {soundPos.X, soundPos.Y, 0.0f};
				MIX_SetTrack3DPosition(Tracks[channel], &position);

				// Output position of each sound emitted so we can verify
				// the sanity of the implementation.
				/*printf("X: %3.3f Y: %3.3f Angle: %3.3f Distance: %3d, Object: %s\n",
				       soundPos.X,
				       soundPos.Y,
				       angleDeg,
				       distance,
				       info
				);*/
			}
		}
		MIX_PlayTrack(Tracks[channel], 0);
	}
}

// void Sound::PlayMusic(MIX_Audio* wavePtr, int time, TPinballComponent *soundSource, const char* info)
// {
// 	if (MixOpen && wavePtr && enabled_flag)
// 	{
//
// 	}
// }

MIX_Audio* Sound::LoadWaveFile(const std::string& lpName)
{
	if (!MixOpen)
	{
		return nullptr;
	}

	auto wavFile = fopenu(lpName.c_str(), "r");
	if (!wavFile)
	{
		return nullptr;
	}
	fclose(wavFile);

	return MIX_LoadAudio(mixer, lpName.c_str(), false);
}

void Sound::FreeSound(MIX_Audio* wave)
{
	if (MixOpen && wave)
	{
		MIX_DestroyAudio(wave);
	}
}

void Sound::SetChannels(int channels)
{
	if (channels <= 0)
	{
		channels = 8;
	}

	num_channels = channels;
	Channels.resize(num_channels);
	if (MixOpen)
	{
		if (!mixer)
		{
			mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
		}

		for (int i = 0; i < num_channels; ++i)
		{
			Tracks[i] = MIX_CreateTrack(mixer);
			MIX_SetTrackGain(Tracks[i], Volume);
		}
	}
	SetVolume(Volume);
}

void Sound::SetVolume(float volume)
{
	if (volume > 1.0f)
	{
		volume = volume / MIX_MAX_VOLUME;
	}

	Volume = volume;

	if (MixOpen)
	{
		for (int i = 0; i < num_channels; ++i)
		{
			MIX_SetTrackGain(Tracks[i], Volume);
		}
	}
}


void Sound::Shutdown()
{
	for (int i = 0; i < num_channels; ++i)
	{
		MIX_DestroyTrack(Tracks[i]);
	}
}
