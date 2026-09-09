#include "lpch.h"
#include "AudioEventInstance.h"

#include "AudioEngine.h"

#ifdef LUX_ENABLE_FMOD
	#include <fmod.hpp>
	#include <fmod_studio.hpp>
	#include <fmod_errors.h>

	#include <algorithm>
	#include <cstdio>
#endif

namespace Lux {

#ifdef LUX_ENABLE_FMOD

	namespace {

		// The inverse of AudioEngine's GuidToString: scenes store the braced form fmodstudiocl
		// exports, and Studio resolves events from an FMOD_GUID.
		bool ParseGuid(const std::string& text, FMOD_GUID& outGuid)
		{
			unsigned int data1 = 0;
			unsigned int data2 = 0, data3 = 0;
			unsigned int d[8] = {};

			const int matched = std::sscanf(text.c_str(), "{%8x-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x}",
				&data1, &data2, &data3, &d[0], &d[1], &d[2], &d[3], &d[4], &d[5], &d[6], &d[7]);
			if (matched != 11)
				return false;

			outGuid.Data1 = data1;
			outGuid.Data2 = (unsigned short)data2;
			outGuid.Data3 = (unsigned short)data3;
			for (int i = 0; i < 8; i++)
				outGuid.Data4[i] = (unsigned char)d[i];

			return true;
		}

		FMOD_VECTOR ToFMOD(const glm::vec3& v)
		{
			return FMOD_VECTOR{ v.x, v.y, v.z };
		}

	}

	Ref<AudioEventInstance> AudioEventInstance::Create(const std::string& eventGuid)
	{
		FMOD::Studio::System* studio = AudioEngine::GetStudioSystem();
		if (!studio || eventGuid.empty())
			return nullptr;

		FMOD_GUID guid{};
		if (!ParseGuid(eventGuid, guid))
		{
			LUX_CORE_ERROR_TAG("Audio", "'{0}' is not a valid event GUID", eventGuid);
			return nullptr;
		}

		FMOD::Studio::EventDescription* description = nullptr;
		if (studio->getEventByID(&guid, &description) != FMOD_OK || !description)
		{
			// Names the GUID rather than staying silent: the usual cause is a scene referencing an
			// event whose bank has not been built, and the GUID is what to search GUIDs.txt for.
			LUX_CORE_WARN_TAG("Audio", "Event {0} is not in any loaded bank - build the FMOD Studio project, or check the bank is loaded", eventGuid);
			return nullptr;
		}

		FMOD::Studio::EventInstance* instance = nullptr;
		if (description->createInstance(&instance) != FMOD_OK || !instance)
		{
			LUX_CORE_ERROR_TAG("Audio", "Failed to create an instance of event {0}", eventGuid);
			return nullptr;
		}

		return Ref<AudioEventInstance>(new AudioEventInstance(instance));
	}

	AudioEventInstance::~AudioEventInstance()
	{
		if (!m_Instance)
			return;

		// Stop before release so a still-audible instance does not keep its resources alive past the
		// scene that owned it. FMOD_STUDIO_STOP_IMMEDIATE rather than ALLOWFADEOUT: this runs during
		// teardown, where a fade would outlive the thing being torn down.
		m_Instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		m_Instance->release();
		m_Instance = nullptr;
	}

	void AudioEventInstance::Start()
	{
		if (m_Instance)
			m_Instance->start();
	}

	void AudioEventInstance::Stop(bool allowFadeOut)
	{
		if (m_Instance)
			m_Instance->stop(allowFadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE);
	}

	void AudioEventInstance::SetPaused(bool paused)
	{
		if (m_Instance)
			m_Instance->setPaused(paused);
	}

	bool AudioEventInstance::IsPlaying() const
	{
		if (!m_Instance)
			return false;

		FMOD_STUDIO_PLAYBACK_STATE state = FMOD_STUDIO_PLAYBACK_STOPPED;
		if (m_Instance->getPlaybackState(&state) != FMOD_OK)
			return false;

		// SUSTAINING counts as playing: the event is holding on a sustain point waiting for a key-off,
		// which is audible even though it is not STARTING or PLAYING.
		return state == FMOD_STUDIO_PLAYBACK_PLAYING
			|| state == FMOD_STUDIO_PLAYBACK_STARTING
			|| state == FMOD_STUDIO_PLAYBACK_SUSTAINING;
	}

	void AudioEventInstance::Set3DAttributes(const glm::vec3& position, const glm::vec3& velocity,
		const glm::vec3& forward, const glm::vec3& up)
	{
		if (!m_Instance)
			return;

		FMOD_3D_ATTRIBUTES attributes{};
		attributes.position = ToFMOD(position);
		attributes.velocity = ToFMOD(velocity);
		attributes.forward = ToFMOD(forward);
		attributes.up = ToFMOD(up);
		m_Instance->set3DAttributes(&attributes);
	}

	void AudioEventInstance::SetVolume(float volume)
	{
		if (m_Instance)
			m_Instance->setVolume(std::max(volume, 0.0f));
	}

	void AudioEventInstance::SetPitch(float pitch)
	{
		if (m_Instance)
			m_Instance->setPitch(std::max(pitch, 0.0f));
	}

	bool AudioEventInstance::SetParameter(const std::string& name, float value)
	{
		if (!m_Instance)
			return false;

		return m_Instance->setParameterByName(name.c_str(), value) == FMOD_OK;
	}

	void AudioEventInstance::SetAcoustics(const AudioSourceAcoustics& acoustics)
	{
		if (!m_Instance)
			return;

		// Collapsed to one 0-1 occlusion amount rather than the Core path's split of volume and
		// filter, because here the engine is not applying the effect - the event is. The low band
		// carries the broadband transmission, so 1 - gainLF is the honest "how blocked is this".
		const float occlusion = std::clamp(1.0f - acoustics.OcclusionGainLF, 0.0f, 1.0f);
		SetParameter(kOcclusionParameter, occlusion);
		SetParameter(kReverbSendParameter, std::clamp(acoustics.ReverbSend, 0.0f, 1.0f));
	}

#else

	// Without FMOD there is no Studio layer at all, so an event instance can never be created and
	// every method below is unreachable rather than merely inert.
	Ref<AudioEventInstance> AudioEventInstance::Create(const std::string&) { return nullptr; }
	AudioEventInstance::~AudioEventInstance() = default;
	void AudioEventInstance::Start() {}
	void AudioEventInstance::Stop(bool) {}
	void AudioEventInstance::SetPaused(bool) {}
	bool AudioEventInstance::IsPlaying() const { return false; }
	void AudioEventInstance::Set3DAttributes(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&) {}
	void AudioEventInstance::SetVolume(float) {}
	void AudioEventInstance::SetPitch(float) {}
	bool AudioEventInstance::SetParameter(const std::string&, float) { return false; }
	void AudioEventInstance::SetAcoustics(const AudioSourceAcoustics&) {}

#endif

}
