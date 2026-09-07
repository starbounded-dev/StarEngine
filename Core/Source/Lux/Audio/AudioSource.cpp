#include "lpch.h"
#include "AudioSource.h"

#include "AudioEngine.h"
#include "Lux/Project/Project.h"

#ifdef LUX_ENABLE_FMOD
	#include <fmod.hpp>
	#include <fmod_errors.h>
#endif

namespace Lux {

#ifdef LUX_ENABLE_FMOD

	namespace {

		// AttenuationModelType::None combined with Spatialization=true has no direct FMOD rolloff
		// flag equivalent (FMOD's flags are all distance-based falloff curves); it's approximated
		// with a flat two-point custom rolloff instead, applied by SetAttenuationModel/SetConfig.
		FMOD_MODE RolloffModeFor(AttenuationModelType model)
		{
			switch (model)
			{
				case AttenuationModelType::None:		return FMOD_3D_CUSTOMROLLOFF;
				case AttenuationModelType::Inverse:		return FMOD_3D_INVERSEROLLOFF;
				case AttenuationModelType::Linear:		return FMOD_3D_LINEARROLLOFF;
				case AttenuationModelType::Exponential: return FMOD_3D_INVERSETAPEREDROLLOFF;
			}

			return FMOD_3D_INVERSEROLLOFF;
		}

		void ApplyFlatRolloffIfNone(FMOD::Sound* sound, AttenuationModelType model, float minDistance, float maxDistance)
		{
			if (model != AttenuationModelType::None || !sound)
				return;

			FMOD_VECTOR flatCurve[2] = {
				{ minDistance, 1.0f, 0.0f },
				{ maxDistance, 1.0f, 0.0f },
			};
			sound->set3DCustomRolloff(flatCurve, 2);
		}

	}

	AudioSource::AudioSource()
	{
		LUX_PROFILE_FUNCTION("AudioSource::AudioSource");
	}

	AudioSource::~AudioSource()
	{
		LUX_PROFILE_FUNCTION("AudioSource::~AudioSource");

		if (m_Channel)
			m_Channel->stop();

		if (m_Sound)
			m_Sound->release();
	}

	bool AudioSource::LoadFromFile(const std::filesystem::path& filepath)
	{
		LUX_PROFILE_FUNCTION("AudioSource::LoadFromFile");

		auto* engine = AudioEngine::GetEngine();
		if (!engine)
			return false;

		if (m_Channel)
		{
			m_Channel->stop();
			m_Channel = nullptr;
		}

		if (m_Sound)
		{
			m_Sound->release();
			m_Sound = nullptr;
			m_IsLoaded = false;
		}

		const FMOD_RESULT result = engine->createSound(filepath.string().c_str(), FMOD_3D | FMOD_LOOP_OFF, nullptr, &m_Sound);
		if (result != FMOD_OK)
		{
			LUX_CORE_ERROR("Failed to initialize sound: {} ({})", filepath.string(), FMOD_ErrorString(result));
			return false;
		}

		m_FilePath = filepath;
		m_IsLoaded = true;
		m_CursorPos = 0;
		return true;
	}

	void AudioSource::Play()
	{
		LUX_PROFILE_FUNCTION("AudioSource::Play");

		if (!m_Sound || !m_IsLoaded)
			return;

		if (!m_Channel)
		{
			auto* engine = AudioEngine::GetEngine();
			if (!engine || engine->playSound(m_Sound, nullptr, true, &m_Channel) != FMOD_OK)
				return;

			// A freshly-created channel starts at the FMOD default (origin); apply whatever
			// position/velocity was set on this AudioSource before Play() was first called.
			FMOD_VECTOR pos{ m_CachedPosition.x, m_CachedPosition.y, m_CachedPosition.z };
			FMOD_VECTOR vel{ m_CachedVelocity.x, m_CachedVelocity.y, m_CachedVelocity.z };
			m_Channel->set3DAttributes(&pos, &vel);
		}

		m_Channel->setPaused(false);
	}

	void AudioSource::Pause()
	{
		LUX_PROFILE_FUNCTION("AudioSource::Pause");

		if (m_Channel)
			m_Channel->setPaused(true);
	}

	void AudioSource::UnPause()
	{
		LUX_PROFILE_FUNCTION("AudioSource::UnPause");

		if (m_Channel)
			m_Channel->setPaused(false);
	}

	void AudioSource::Stop()
	{
		LUX_PROFILE_FUNCTION("AudioSource::Stop");

		// A real Channel::stop() permanently invalidates the channel (FMOD channels are one-shot
		// handles recycled once stopped), which doesn't fit this API's "stopped but replayable in
		// place" contract - callers call Play() again on the same AudioSource later. Rewind and
		// pause the existing channel instead.
		if (m_Channel)
		{
			m_Channel->setPosition(0, FMOD_TIMEUNIT_PCM);
			m_Channel->setPaused(true);
		}

		m_CursorPos = 0;
	}

	bool AudioSource::IsPlaying()
	{
		LUX_PROFILE_FUNCTION("AudioSource::IsPlaying");

		if (!m_Channel)
			return false;

		bool paused = true;
		bool playing = false;
		if (m_Channel->getPaused(&paused) != FMOD_OK || m_Channel->isPlaying(&playing) != FMOD_OK)
		{
			// The channel finished (non-looping) and FMOD recycled the handle for another sound.
			m_Channel = nullptr;
			return false;
		}

		return playing && !paused;
	}

	uint64_t AudioSource::GetCursorPosition()
	{
		LUX_PROFILE_FUNCTION("AudioSource::GetCursorPosition");

		if (!m_Channel)
			return m_CursorPos;

		unsigned int position = 0;
		if (m_Channel->getPosition(&position, FMOD_TIMEUNIT_PCM) == FMOD_OK)
			m_CursorPos = position;
		else
			m_Channel = nullptr;

		return m_CursorPos;
	}

	void AudioSource::SetConfig(const AudioSourceConfig& config)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetConfig");

		if (!m_Sound || !m_IsLoaded)
			return;

		m_Spatialization = config.Spatialization;

		FMOD_MODE mode = config.Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		mode |= config.Spatialization ? (FMOD_3D | RolloffModeFor(config.AttenuationModel)) : FMOD_2D;
		m_Sound->setMode(mode);

		if (config.Spatialization)
		{
			ApplyFlatRolloffIfNone(m_Sound, config.AttenuationModel, config.MinDistance, config.MaxDistance);
			m_Sound->set3DMinMaxDistance(config.MinDistance, config.MaxDistance);
			m_Sound->set3DConeSettings(glm::degrees(config.ConeInnerAngle), glm::degrees(config.ConeOuterAngle), config.ConeOuterGain);
		}

		if (m_Channel)
		{
			m_Channel->setVolume(config.VolumeMultiplier);
			m_Channel->setPitch(config.PitchMultiplier);

			if (config.Spatialization)
			{
				m_Channel->set3DMinMaxDistance(config.MinDistance, config.MaxDistance);
				m_Channel->set3DConeSettings(glm::degrees(config.ConeInnerAngle), glm::degrees(config.ConeOuterAngle), config.ConeOuterGain);
				m_Channel->set3DDopplerLevel(std::max(config.DopplerFactor, 0.0f));
			}
		}
	}

	void AudioSource::SetVolume(float volume)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetVolume");

		if (m_Channel)
			m_Channel->setVolume(volume);
	}

	void AudioSource::SetPitch(float pitch)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetPitch");

		if (m_Channel)
			m_Channel->setPitch(pitch);
	}

	bool AudioSource::IsLooping()
	{
		if (!m_Sound)
			return false;

		FMOD_MODE mode = FMOD_DEFAULT;
		m_Sound->getMode(&mode);
		return (mode & FMOD_LOOP_NORMAL) != 0;
	}

	void AudioSource::SetLooping(bool state)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetLooping");

		if (m_Sound)
			m_Sound->setMode(state ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	}

	void AudioSource::SetSpatialization(bool state)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetSpatialization");

		m_Spatialization = state;
		if (m_Sound)
			m_Sound->setMode(state ? FMOD_3D : FMOD_2D);
	}

	void AudioSource::SetAttenuationModel(AttenuationModelType type)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetAttenuationModel");

		if (!m_Sound || !m_Spatialization)
			return;

		m_Sound->setMode(FMOD_3D | RolloffModeFor(type));

		float minDistance = 1.0f, maxDistance = 1000.0f;
		m_Sound->get3DMinMaxDistance(&minDistance, &maxDistance);
		ApplyFlatRolloffIfNone(m_Sound, type, minDistance, maxDistance);
	}

	void AudioSource::SetRollOff(float rollOff)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetRollOff");
		// No direct FMOD equivalent to miniaudio's continuous rolloff scalar - the discrete
		// rolloff curve selected by SetAttenuationModel is the closest match.
		(void)rollOff;
	}

	void AudioSource::SetMinGain(float minGain)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetMinGain");
		// No direct FMOD equivalent - min/max gain clamp the attenuation curve's output in
		// miniaudio, independent of min/max distance. Unimplemented for FMOD.
		(void)minGain;
	}

	void AudioSource::SetMaxGain(float maxGain)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetMaxGain");
		(void)maxGain;
	}

	void AudioSource::SetMinDistance(float minDistance)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetMinDistance");

		if (!m_Sound)
			return;

		float min = minDistance, max = 1000.0f;
		m_Sound->get3DMinMaxDistance(&min, &max);
		m_Sound->set3DMinMaxDistance(minDistance, max);
		if (m_Channel)
			m_Channel->set3DMinMaxDistance(minDistance, max);
	}

	void AudioSource::SetMaxDistance(float maxDistance)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetMaxDistance");

		if (!m_Sound)
			return;

		float min = 0.3f, max = maxDistance;
		m_Sound->get3DMinMaxDistance(&min, &max);
		m_Sound->set3DMinMaxDistance(min, maxDistance);
		if (m_Channel)
			m_Channel->set3DMinMaxDistance(min, maxDistance);
	}

	void AudioSource::SetCone(float innerAngle, float outerAngle, float outerGain)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetCone");

		const float innerDegrees = glm::degrees(innerAngle);
		const float outerDegrees = glm::degrees(outerAngle);

		if (m_Sound)
			m_Sound->set3DConeSettings(innerDegrees, outerDegrees, outerGain);
		if (m_Channel)
			m_Channel->set3DConeSettings(innerDegrees, outerDegrees, outerGain);
	}

	void AudioSource::SetDopplerFactor(float factor)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetDopplerFactor");

		if (m_Channel)
			m_Channel->set3DDopplerLevel(std::max(factor, 0.0f));
	}

	void AudioSource::SetPosition(const glm::vec4& position)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetPosition");

		m_CachedPosition = glm::vec3(position);
		if (!m_Channel)
			return;

		FMOD_VECTOR pos{ m_CachedPosition.x, m_CachedPosition.y, m_CachedPosition.z };
		FMOD_VECTOR vel{ m_CachedVelocity.x, m_CachedVelocity.y, m_CachedVelocity.z };
		m_Channel->set3DAttributes(&pos, &vel);
	}

	void AudioSource::SetDirection(const glm::vec3& forward)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetDirection");

		if (m_Channel)
		{
			FMOD_VECTOR direction{ forward.x, forward.y, forward.z };
			m_Channel->set3DConeOrientation(&direction);
		}
	}

	void AudioSource::SetVelocity(const glm::vec3& velocity)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetVelocity");

		m_CachedVelocity = velocity;
		if (!m_Channel)
			return;

		FMOD_VECTOR pos{ m_CachedPosition.x, m_CachedPosition.y, m_CachedPosition.z };
		FMOD_VECTOR vel{ m_CachedVelocity.x, m_CachedVelocity.y, m_CachedVelocity.z };
		m_Channel->set3DAttributes(&pos, &vel);
	}

	void AudioSource::SetOcclusion(float directOcclusion, float reverbOcclusion)
	{
		if (m_Channel)
			m_Channel->set3DOcclusion(directOcclusion, reverbOcclusion);
	}

	void AudioSource::SetReverbSend(float wet)
	{
		if (m_Channel)
			m_Channel->setReverbProperties(0, wet);
	}

#else

	AudioSource::AudioSource()
	{
		LUX_PROFILE_FUNCTION("AudioSource::AudioSource");

		m_Sound = std::make_unique<ma_sound>();
	}

	AudioSource::~AudioSource()
	{
		LUX_PROFILE_FUNCTION("AudioSource::~AudioSource");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_stop(m_Sound.get());
			ma_sound_uninit(m_Sound.get());
			m_IsLoaded = false;
		}
	}

	bool AudioSource::LoadFromFile(const std::filesystem::path& filepath)
	{
		LUX_PROFILE_FUNCTION("AudioSource::LoadFromFile");

		auto* engine = AudioEngine::GetEngine();
		if (!engine)
			return false;

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_stop(m_Sound.get());
			ma_sound_uninit(m_Sound.get());
			m_IsLoaded = false;
		}

		const ma_result result = ma_sound_init_from_file(engine, filepath.string().c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, nullptr, m_Sound.get());
		if (result != MA_SUCCESS)
		{
			LUX_CORE_ERROR("Failed to initialize sound: {}", filepath.string());
			return false;
		}

		m_FilePath = filepath;
		m_IsLoaded = true;
		m_CursorPos = 0;
		return true;
	}

	void AudioSource::Play()
	{
		LUX_PROFILE_FUNCTION("AudioSource::Play");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_start(m_Sound.get());
		}
	}

	void AudioSource::Pause()
	{
		LUX_PROFILE_FUNCTION("AudioSource::Pause");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_stop(m_Sound.get());
		}
	}

	void AudioSource::UnPause()
	{
		LUX_PROFILE_FUNCTION("AudioSource::UnPause");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_start(m_Sound.get());
		}
	}

	void AudioSource::Stop()
	{
		LUX_PROFILE_FUNCTION("AudioSource::Stop");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_stop(m_Sound.get());
			ma_sound_seek_to_pcm_frame(m_Sound.get(), 0);

			m_CursorPos = 0;
		}
	}

	bool AudioSource::IsPlaying()
	{
		LUX_PROFILE_FUNCTION("AudioSource::IsPlaying");

		if (m_Sound.get() && m_IsLoaded)
			return ma_sound_is_playing(m_Sound.get());

		return false;
	}

	uint64_t AudioSource::GetCursorPosition()
	{
		if (m_Sound.get() && m_IsLoaded)
		{
			//uint64_t cursorPos = 0;
			ma_sound_get_cursor_in_pcm_frames(m_Sound.get(), &m_CursorPos);
			return m_CursorPos;
		}

		return 0;
	}

	static ma_attenuation_model GetAttenuationModel(AttenuationModelType model)
	{
		LUX_PROFILE_FUNCTION("ma_attenuation_model GetAttenuationModel");

		switch (model)
		{
		case AttenuationModelType::None:		return ma_attenuation_model_none;
		case AttenuationModelType::Inverse:		return ma_attenuation_model_inverse;
		case AttenuationModelType::Linear:		return ma_attenuation_model_linear;
		case AttenuationModelType::Exponential: return ma_attenuation_model_exponential;
		}

		return ma_attenuation_model_none;
	}

	void AudioSource::SetConfig(const AudioSourceConfig& config)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetConfig");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound* sound = m_Sound.get();
			ma_sound_set_volume(sound, config.VolumeMultiplier);
			ma_sound_set_pitch(sound, config.PitchMultiplier);

			if (sound)
			{
				if (config.Looping)
					ma_sound_set_looping(sound, MA_TRUE);
				else
					ma_sound_set_looping(sound, MA_FALSE);
			}

			if (m_Spatialization != config.Spatialization)
			{
				m_Spatialization = config.Spatialization;
				ma_sound_set_spatialization_enabled(sound, config.Spatialization);
			}

			if (config.Spatialization)
			{
				ma_sound_set_attenuation_model(sound, GetAttenuationModel(config.AttenuationModel));
				ma_sound_set_rolloff(sound, config.RollOff);
				ma_sound_set_min_gain(sound, config.MinGain);
				ma_sound_set_max_gain(sound, config.MaxGain);
				ma_sound_set_min_distance(sound, config.MinDistance);
				ma_sound_set_max_distance(sound, config.MaxDistance);

				ma_sound_set_cone(sound, config.ConeInnerAngle, config.ConeOuterAngle, config.ConeOuterGain);
				ma_sound_set_doppler_factor(sound, std::max(config.DopplerFactor, 0.0f));
			}
			else
			{
				ma_sound_set_attenuation_model(sound, ma_attenuation_model_none);
			}
		}
	}

	void AudioSource::SetVolume(float volume)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetVolume");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_volume(m_Sound.get(), volume);
		}
	}

	void AudioSource::SetPitch(float pitch)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetPitch");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_pitch(m_Sound.get(), pitch);
		}
	}

	bool AudioSource::IsLooping()
	{
		if (m_Sound && m_IsLoaded)
			return ma_sound_is_looping(m_Sound.get());

		return false;
	}

	void AudioSource::SetLooping(bool state)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetLooping");

		if (m_Sound && m_IsLoaded)
		{
			if (state)
				ma_sound_set_looping(m_Sound.get(), MA_TRUE);
			else
				ma_sound_set_looping(m_Sound.get(), MA_FALSE);
		}
	}

	void AudioSource::SetSpatialization(bool state)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetSpatialization");

		m_Spatialization = state;
		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_spatialization_enabled(m_Sound.get(), state);
		}
	}

	void AudioSource::SetAttenuationModel(AttenuationModelType type)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetAttenuationModel");

		if (m_Sound && m_IsLoaded)
		{
			if (m_Spatialization)
				ma_sound_set_attenuation_model(m_Sound.get(), GetAttenuationModel(type));
			else
				ma_sound_set_attenuation_model(m_Sound.get(), GetAttenuationModel(AttenuationModelType::None));
		}
	}

	void AudioSource::SetRollOff(float rollOff)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetRollOff");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_rolloff(m_Sound.get(), rollOff);
		}
	}

	void AudioSource::SetMinGain(float minGain)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetRollOff");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_min_gain(m_Sound.get(), minGain);
		}
	}

	void AudioSource::SetMaxGain(float maxGain)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetRollOff");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_max_gain(m_Sound.get(), maxGain);
		}
	}

	void AudioSource::SetMinDistance(float minDistance)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetRollOff");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_min_distance(m_Sound.get(), minDistance);
		}
	}

	void AudioSource::SetMaxDistance(float maxDistance)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetRollOff");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_max_distance(m_Sound.get(), maxDistance);
		}
	}

	void AudioSource::SetCone(float innerAngle, float outerAngle, float outerGain)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetRollOff");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_cone(m_Sound.get(), innerAngle, outerAngle, outerGain);
		}
	}

	void AudioSource::SetDopplerFactor(float factor)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetDopplerFactor");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_doppler_factor(m_Sound.get(), std::max(factor, 0.0f));
		}
	}

	void AudioSource::SetPosition(const glm::vec4& position)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetPosition");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_position(m_Sound.get(), position.x, position.y, position.z);
		}
	}

	void AudioSource::SetDirection(const glm::vec3& forward)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetDirection");

		if (m_Sound && m_IsLoaded)
		{
			ma_sound_set_direction(m_Sound.get(), forward.x, forward.y, forward.z);
		}
	}

	void AudioSource::SetVelocity(const glm::vec3& velocity)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetVelocity");

		if (m_Sound)
		{
			ma_sound_set_velocity(m_Sound.get(), velocity.x, velocity.y, velocity.z);
		}
	}

	void AudioSource::SetOcclusion(float directOcclusion, float reverbOcclusion)
	{
		// miniaudio has no built-in occlusion/lowpass-per-source equivalent to drive here.
		(void)directOcclusion;
		(void)reverbOcclusion;
	}

	void AudioSource::SetReverbSend(float wet)
	{
		(void)wet;
	}

#endif

}
