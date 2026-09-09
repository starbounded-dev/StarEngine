#pragma once

#include "Lux/Core/Ref.h"

#include <filesystem>
#include <string>

#ifdef LUX_ENABLE_FMOD
namespace FMOD { class Sound; class Channel; }
#else
#include "miniaudio.h"
struct ma_sound;
#endif

namespace Lux {

	// Playback settings for a source.
	//
	// Deliberately small. Everything that shapes how a sound is *heard* - distance attenuation,
	// cones, doppler, randomisation, filtering - is authored on an FMOD Studio event, where a sound
	// designer can change it without a rebuild. Duplicating those as component fields made them
	// visible, editable and silently ignored the moment an event was assigned.
	//
	// What remains is what gameplay legitimately drives per entity, and what the legacy raw-file
	// path needs to function at all.
	struct AudioSourceConfig
	{
		float VolumeMultiplier = 1.0f;
		float PitchMultiplier = 1.0f;
		bool PlayOnAwake = true;

		// Legacy path only - an event loops via a loop region on its timeline.
		bool Looping = false;
	};

	// Per-source output of the ray-traced acoustics simulation, as linear gains (1 = unaffected).
	// Two-band because that is what the simulation measures and what makes occlusion sound like
	// muffling rather than a volume knob.
	struct AudioSourceAcoustics
	{
		// Direct path from the listener to this source.
		float OcclusionGainLF = 1.0f;
		float OcclusionGainHF = 1.0f;

		// How enclosed the listener is, which muffles the reverb this source feeds.
		float AmbientGainLF = 1.0f;
		float AmbientGainHF = 1.0f;

		// How much of this source's energy the space returns (0 = anechoic, 1 = fully wet).
		float ReverbSend = 0.0f;
	};

	class AudioSource : public RefCounted
	{
	public:
		AudioSource();
		~AudioSource();

		bool LoadFromFile(const std::filesystem::path& filepath);
		bool IsLoaded() const { return m_IsLoaded; }
		const std::filesystem::path& GetFilePath() const { return m_FilePath; }

		void Play();
		void Pause();
		void UnPause();
		void Stop();
		bool IsPlaying();
		uint64_t GetCursorPosition();

		void SetConfig(const AudioSourceConfig& config);

		void SetVolume(float volume);
		void SetPitch(float pitch);
		bool IsLooping();
		void SetLooping(bool state);

		void SetPosition(const glm::vec4& position);
		void SetDirection(const glm ::vec3& forward);
		void SetVelocity(const glm ::vec3& velocity);

		// Applied by RaytracedAudioScene's per-frame sync (Scene::OnUpdateRuntime). A no-op under
		// miniaudio, which has no per-source occlusion or reverb send to drive.
		//
		// Occlusion interacts with SetVolume: the low-frequency gain scales the source's level,
		// so both go through the same effective-volume calculation and neither clobbers the other.
		void SetAcoustics(const AudioSourceAcoustics& acoustics);

		// The backend's own final audibility for this voice, 0 to 1, with 3D attenuation, occlusion
		// and bus levels already folded in. Negative when the backend cannot report it. This is the
		// number that answers "is spatialisation actually doing anything" - it tracks the rolloff
		// curve as the listener moves, and a value that does not change with distance means the 3D
		// path is inert.
		float GetAudibility() const;

	private:
#ifdef LUX_ENABLE_FMOD
		FMOD::Sound* m_Sound = nullptr;
		FMOD::Channel* m_Channel = nullptr;
		// FMOD combines position+velocity, and separately direction, into single calls - cache
		// whichever was set last so each individual setter can resend a complete state.
		glm::vec3 m_CachedPosition{ 0.0f };
		glm::vec3 m_CachedVelocity{ 0.0f };
		uint64_t m_CursorPos = 0;
		// The channel's volume is the product of these two: what the component asked for, and what
		// the acoustics simulation says survives the trip to the listener. Kept apart so a config
		// change doesn't wipe out occlusion, and occlusion doesn't overwrite the authored volume.
		float m_ConfiguredVolume = 1.0f;
		float m_OcclusionVolumeScale = 1.0f;
#else
		std::unique_ptr<ma_sound> m_Sound;
		// ma_uint64 (unsigned long long) and uint64_t (unsigned long on LP64) are distinct types
		// here, and ma_sound_get_cursor_in_pcm_frames takes ma_uint64* - keep its native type.
		ma_uint64 m_CursorPos = 0;
#endif
		std::filesystem::path m_FilePath;
		bool m_IsLoaded = false;
	};
}
