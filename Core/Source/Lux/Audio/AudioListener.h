#pragma once

#include <stdint.h>

namespace Lux {

	struct AudioListenerConfig
	{
		float ConeInnerAngle = glm::radians(360.0f);
		float ConeOuterAngle = glm::radians(360.0f);
		float ConeOuterGain = 0.0f;
	};

	class AudioListener : public RefCounted
	{
	public:
		AudioListener() = default;

		void SetConfig(const AudioListenerConfig& config) const;
		void SetPosition(const glm::vec4& position) const;
		void SetDirection(const glm::vec3& forward) const;
		void SetVelocity(const glm::vec3& velocity) const;
		//void SetPosition(const glm::vec3& position) const;
		//void SetDirection(const glm::vec3& forward) const;
		//void SetVelocity(const glm::vec3& velocity) const;

	private:
		uint32_t m_ListenerIndex = 0;
#ifdef LUX_ENABLE_FMOD
		// FMOD's System::set3DListenerAttributes() takes position/velocity/forward/up together;
		// each individual setter below resends this whole cached state.
		mutable glm::vec3 m_CachedPosition{ 0.0f };
		mutable glm::vec3 m_CachedVelocity{ 0.0f };
		mutable glm::vec3 m_CachedForward{ 0.0f, 0.0f, -1.0f };
#endif
	};
}
