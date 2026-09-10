#include "lpch.h"
#include "AudioSource.h"

#include "AudioEngine.h"
#include "Lux/Project/Project.h"

#include <fmod.hpp>
#include <fmod_errors.h>

namespace Lux {


	namespace {

		// How much more the high band is attenuated than the low band, as 0 (no spectral tilt) to
		// 1 (highs gone entirely). This is the part of a two-band measurement that a single-scalar
		// occlusion control can carry without also re-applying the broadband loss, which the
		// caller has already handled by scaling the channel's volume.
		float RelativeHighFrequencyLoss(float gainLF, float gainHF)
		{
			// Below this the low band is inaudible anyway, and the ratio becomes numerically
			// meaningless — treat it as no additional filtering rather than dividing by ~0.
			constexpr float minAudibleGain = 1e-4f;
			if (gainLF <= minAudibleGain)
				return 0.0f;

			return std::clamp(1.0f - (gainHF / gainLF), 0.0f, 1.0f);
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

		// Always 3D, matching how the sound was created. The old Spatialization flag defaulted to
		// off, which quietly made every raw source 2D; tuning raw-file attenuation is exactly the
		// thing an event should be doing instead, so the rolloff is left at FMOD's defaults.
		FMOD_MODE mode = config.Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		mode |= FMOD_3D;
		m_Sound->setMode(mode);

		m_ConfiguredVolume = config.VolumeMultiplier;

		if (m_Channel)
		{
			m_Channel->setVolume(m_ConfiguredVolume * m_OcclusionVolumeScale);
			m_Channel->setPitch(config.PitchMultiplier);
		}
	}

	void AudioSource::SetVolume(float volume)
	{
		LUX_PROFILE_FUNCTION("AudioSource::SetVolume");

		m_ConfiguredVolume = volume;

		if (m_Channel)
			m_Channel->setVolume(m_ConfiguredVolume * m_OcclusionVolumeScale);
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

	void AudioSource::SetAcoustics(const AudioSourceAcoustics& acoustics)
	{
		if (!m_Channel)
			return;

		const float occlusionGainLF = std::clamp(acoustics.OcclusionGainLF, 0.0f, 1.0f);
		const float occlusionGainHF = std::clamp(acoustics.OcclusionGainHF, 0.0f, 1.0f);
		const float ambientGainLF = std::clamp(acoustics.AmbientGainLF, 0.0f, 1.0f);
		const float ambientGainHF = std::clamp(acoustics.AmbientGainHF, 0.0f, 1.0f);

		// FMOD's occlusion is a single scalar that attenuates *and* low-passes together, so the two
		// measured bands are split across the two controls that can carry them independently:
		//
		//   level  <- the low-frequency gain, which is how much sound gets through at all;
		//   filter <- how much *further* the highs are attenuated relative to the lows.
		//
		// Feeding the HF gain straight into set3DOcclusion instead would double-count the loss:
		// FMOD would attenuate by the HF amount as well as filtering by it, so a source behind a
		// wall would go inaudible rather than muffled.
		m_OcclusionVolumeScale = occlusionGainLF;
		m_Channel->setVolume(m_ConfiguredVolume * m_OcclusionVolumeScale);

		m_Channel->set3DOcclusion(RelativeHighFrequencyLoss(occlusionGainLF, occlusionGainHF),
			RelativeHighFrequencyLoss(ambientGainLF, ambientGainHF));

		m_Channel->setReverbProperties(0, std::clamp(acoustics.ReverbSend, 0.0f, 1.0f));
	}

	float AudioSource::GetAudibility() const
	{
		if (!m_Channel)
			return -1.0f;

		float audibility = -1.0f;
		if (m_Channel->getAudibility(&audibility) != FMOD_OK)
			return -1.0f;

		return audibility;
	}


}
