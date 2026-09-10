#include "lpch.h"
#include "AudioListener.h"

#include "AudioEngine.h"

#include <algorithm>
#include <cmath>

#include <fmod_studio.hpp>
#include <fmod_errors.h>

namespace Lux {

	namespace {

		constexpr double kMinUpLength = 1.0e-6;
		constexpr double kParallelUpThreshold = 0.9;

		bool IsFinite(const glm::vec3& value)
		{
			return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
		}

		// Distinct slots/operations must not reset each other's error suppression each frame.
		std::array<std::array<FMOD_RESULT, AudioListener::MaxListeners>, 7> s_LastResults{};

		bool CheckListenerResult(FMOD_RESULT result, int operation, int index, const char* name)
		{
			auto& last = s_LastResults[operation][index];
			if (result != FMOD_OK && result != last)
				LUX_CORE_ERROR_TAG("Audio", "Listener {0}: {1}: {2}", index, name, FMOD_ErrorString(result));
			last = result;
			return result == FMOD_OK;
		}
	}

	bool AudioListener::SetTransform(AudioListenerState& listener, const glm::mat4& transform)
	{
		if (!IsFinite(glm::vec3(transform[3])) || !IsFinite(glm::vec3(transform[2])) || !IsFinite(glm::vec3(transform[1])))
			return false;

		// World-space columns preserve parent rotations. Remove scale/shear and handle collapsed
		// axes without sending NaNs or a non-orthonormal frame to either mixer.
		glm::dvec3 forward = -glm::dvec3(transform[2]);
		forward = glm::length(forward) > 0.0 ? glm::normalize(forward) : glm::dvec3(0.0, 0.0, -1.0);
		glm::dvec3 up = glm::dvec3(transform[1]);
		if (glm::length(up) > 0.0)
			up = glm::normalize(up);
		up -= forward * glm::dot(up, forward);
		if (glm::length(up) < kMinUpLength)
		{
			up = std::abs(forward.y) < kParallelUpThreshold ? glm::dvec3(0.0, 1.0, 0.0) : glm::dvec3(1.0, 0.0, 0.0);
			up -= forward * glm::dot(up, forward);
		}
		listener.Position = glm::vec3(transform[3]);
		listener.Forward = glm::vec3(forward);
		listener.Up = glm::vec3(glm::normalize(up));
		return true;
	}

	int AudioListener::GetPrimaryIndex(const States& listeners)
	{
		int primary = -1;
		for (int index = 0; index < MaxListeners; ++index)
		{
			if (std::isfinite(listeners[index].Weight) && listeners[index].Weight > 0.0f &&
				(primary < 0 || listeners[index].Weight > listeners[primary].Weight))
				primary = index;
		}
		return primary;
	}

	void AudioListener::Apply(const States& listeners)
	{
		LUX_PROFILE_FUNCTION("AudioListener::Apply");
		if (!AudioEngine::HasInitializedEngine())
			return;

		const int primary = GetPrimaryIndex(listeners);
		const AudioListenerState fallback;
		const auto& legacy = primary >= 0 ? listeners[primary] : fallback;
		auto* studio = AudioEngine::GetStudioSystem();
		auto* core = AudioEngine::GetEngine();
		int count = 1;
		float totalWeight = 0.0f;
		for (int index = 0; index < MaxListeners; ++index)
		{
			if (std::isfinite(listeners[index].Weight) && listeners[index].Weight > 0.0f)
			{
				count = index + 1;
				totalWeight += std::clamp(listeners[index].Weight, 0.0f, 1.0f);
			}
		}
		int currentCount = 0;
		if (!CheckListenerResult(studio->getNumListeners(&currentCount), 0, 0, "getNumListeners"))
			return;
		if (currentCount != count && !CheckListenerResult(studio->setNumListeners(count), 1, 0, "setNumListeners"))
			return;

		// Studio also owns Core's listener count and rewrites it on its asynchronous update.
		// Mirror the same slots in both APIs; giving Core an independent count races Studio.
		bool coreReady = CheckListenerResult(core->get3DNumListeners(&currentCount), 4, 0, "get3DNumListeners");
		if (coreReady && currentCount != count)
			coreReady = CheckListenerResult(core->set3DNumListeners(count), 5, 0, "set3DNumListeners");

		for (int index = 0; index < count; ++index)
		{
			const float authoredWeight = std::isfinite(listeners[index].Weight) ? std::clamp(listeners[index].Weight, 0.0f, 1.0f) : 0.0f;
			// Core has no listener weights. Put unused slots at an active camera so they cannot
			// create phantom ears at the origin when Studio publishes these attributes to Core.
			const auto& state = authoredWeight > 0.0f ? listeners[index] : legacy;
			const FMOD_3D_ATTRIBUTES attributes{
				{ state.Position.x, state.Position.y, state.Position.z },
				{ state.Velocity.x, state.Velocity.y, state.Velocity.z },
				{ state.Forward.x, state.Forward.y, state.Forward.z },
				{ state.Up.x, state.Up.y, state.Up.z }
			};
			const FMOD_VECTOR attenuation{ state.AttenuationPosition.x, state.AttenuationPosition.y, state.AttenuationPosition.z };
			CheckListenerResult(studio->setListenerAttributes(index, &attributes, state.UseAttenuationPosition ? &attenuation : nullptr),
				2, index, "setListenerAttributes");
			// Studio requires a nonzero total. Weights are relative; an empty scene uses a neutral
			// origin listener so a deleted camera cannot leave a stale location behind.
			const float weight = primary < 0 ? 1.0f : authoredWeight / totalWeight;
			CheckListenerResult(studio->setListenerWeight(index, weight), 3, index, "setListenerWeight");
			if (coreReady)
				CheckListenerResult(core->set3DListenerAttributes(index, &attributes.position, &attributes.velocity, &attributes.forward, &attributes.up),
					6, index, "set3DListenerAttributes");
		}
	}
}
