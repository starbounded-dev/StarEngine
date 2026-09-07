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

	enum class AttenuationModelType
	{
		None = 0,
		Inverse,
		Linear,
		Exponential
	};

	struct AudioSourceConfig
	{
		float VolumeMultiplier = 1.0f;
		float PitchMultiplier = 1.0f;
		bool PlayOnAwake = true;
		bool Looping = false;

		bool Spatialization = false;
		AttenuationModelType AttenuationModel = AttenuationModelType::Inverse;
		float RollOff = 1.0f;
		float MinGain = 0.0f;
		float MaxGain = 1.0f;
		float MinDistance = 0.3f;
		float MaxDistance = 1000.0f;

		float ConeInnerAngle = glm::radians(360.0f);
		float ConeOuterAngle = glm::radians(360.0f);
		float ConeOuterGain = 0.0f;

		float DopplerFactor = 1.0f;
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
		void SetSpatialization(bool state);
		void SetAttenuationModel(AttenuationModelType type);
		void SetRollOff(float rollOff);
		void SetMinGain(float minGain);
		void SetMaxGain(float maxGain);
		void SetMinDistance(float minDistance);
		void SetMaxDistance(float maxDistance);
		void SetCone(float innerAngle, float outerAngle, float outerGain);
		void SetDopplerFactor(float factor);

		void SetPosition(const glm::vec4& position);
		void SetDirection(const glm ::vec3& forward);
		void SetVelocity(const glm ::vec3& velocity);

		// Applied by RaytracedAudioScene's per-frame sync (Scene::OnUpdateRuntime). No-ops under
		// miniaudio, which has no built-in occlusion/reverb-send equivalent.
		void SetOcclusion(float directOcclusion, float reverbOcclusion);
		void SetReverbSend(float wet);

	private:
#ifdef LUX_ENABLE_FMOD
		FMOD::Sound* m_Sound = nullptr;
		FMOD::Channel* m_Channel = nullptr;
		// FMOD combines position+velocity, and separately direction, into single calls - cache
		// whichever was set last so each individual setter can resend a complete state.
		glm::vec3 m_CachedPosition{ 0.0f };
		glm::vec3 m_CachedVelocity{ 0.0f };
		uint64_t m_CursorPos = 0;
#else
		std::unique_ptr<ma_sound> m_Sound;
		// ma_uint64 (unsigned long long) and uint64_t (unsigned long on LP64) are distinct types
		// here, and ma_sound_get_cursor_in_pcm_frames takes ma_uint64* - keep its native type.
		ma_uint64 m_CursorPos = 0;
#endif
		std::filesystem::path m_FilePath;
		bool m_Spatialization = false;
		bool m_IsLoaded = false;
	};
}
