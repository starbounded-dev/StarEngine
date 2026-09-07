#include "lpch.h"
#include "AudioListener.h"

#include "AudioEngine.h"

#ifdef LUX_ENABLE_FMOD
	#include <fmod.hpp>
#else
	#include <miniaudio.h>
#endif

namespace Lux {

#ifdef LUX_ENABLE_FMOD

	namespace {

		void SendListenerAttributes(FMOD::System* engine, int listenerIndex, const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward)
		{
			if (!engine)
				return;

			const FMOD_VECTOR pos{ position.x, position.y, position.z };
			const FMOD_VECTOR vel{ velocity.x, velocity.y, velocity.z };
			const FMOD_VECTOR fwd{ forward.x, forward.y, forward.z };
			const FMOD_VECTOR up{ 0.0f, 1.0f, 0.0f };
			engine->set3DListenerAttributes(listenerIndex, &pos, &vel, &fwd, &up);
		}

	}

	void AudioListener::SetConfig(const AudioListenerConfig& config) const
	{
		LUX_PROFILE_FUNCTION("AudioListener::SetConfig");
		// FMOD has no listener-level cone equivalent to miniaudio's - listener cones apply to
		// emitter-relative attenuation per Channel (see AudioSource::SetCone), not the listener.
		(void)config;
	}

	void AudioListener::SetPosition(const glm::vec4& position) const
	{
		LUX_PROFILE_FUNCTION("AudioListener::SetPosition");

		m_CachedPosition = glm::vec3(position);
		SendListenerAttributes(AudioEngine::GetEngine(), (int)m_ListenerIndex, m_CachedPosition, m_CachedVelocity, m_CachedForward);
	}

	void AudioListener::SetDirection(const glm::vec3& forward) const
	{
		LUX_PROFILE_FUNCTION("AudioListener::SetDirection");

		m_CachedForward = forward;
		SendListenerAttributes(AudioEngine::GetEngine(), (int)m_ListenerIndex, m_CachedPosition, m_CachedVelocity, m_CachedForward);
	}

	void AudioListener::SetVelocity(const glm::vec3& velocity) const
	{
		LUX_PROFILE_FUNCTION("AudioListener::SetVelocity");

		m_CachedVelocity = velocity;
		SendListenerAttributes(AudioEngine::GetEngine(), (int)m_ListenerIndex, m_CachedPosition, m_CachedVelocity, m_CachedForward);
	}

#else

	void AudioListener::SetConfig(const AudioListenerConfig& config) const
	{
		LUX_PROFILE_FUNCTION("AudioListener::SetConfig");

		auto* engine = static_cast<ma_engine*>(AudioEngine::GetEngine());
		ma_engine_listener_set_cone(engine, m_ListenerIndex, config.ConeInnerAngle, config.ConeOuterAngle, config.ConeOuterGain);
	}

	void AudioListener::SetPosition(const glm::vec4& position) const
	{
		LUX_PROFILE_FUNCTION("AudioListener::SetPosition");

		auto* engine = static_cast<ma_engine*>(AudioEngine::GetEngine());
		ma_engine_listener_set_position(engine, m_ListenerIndex, position.x, position.y, position.z);

		static bool setupWorldUp = false;
		if (!setupWorldUp)
		{
			ma_engine_listener_set_world_up(engine, m_ListenerIndex, 0, 1, 0);
			setupWorldUp = true;
		}
	}

	void AudioListener::SetDirection(const glm::vec3& forward) const
	{
		LUX_PROFILE_FUNCTION("AudioListener::SetDirection");

		auto* engine = static_cast<ma_engine*>(AudioEngine::GetEngine());
		ma_engine_listener_set_direction(engine, m_ListenerIndex, forward.x, forward.y, forward.z);
	}

	void AudioListener::SetVelocity(const glm::vec3& velocity) const
	{
		LUX_PROFILE_FUNCTION("AudioListener::SetVelocity");

		auto* engine = static_cast<ma_engine*>(AudioEngine::GetEngine());
		ma_engine_listener_set_velocity(engine, m_ListenerIndex, velocity.x, velocity.y, velocity.z);
	}

#endif

}
