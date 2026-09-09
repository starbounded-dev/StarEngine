#pragma once

#include "Lux/Audio/AudioSource.h"   // AudioSourceAcoustics
#include "Lux/Core/Ref.h"

#include <string>

namespace FMOD { namespace Studio { class EventInstance; } }

namespace Lux {

	// One playing instance of an FMOD Studio event.
	//
	// This is the Studio-era counterpart to AudioSource: where AudioSource owns a Sound and a
	// Channel and the engine decides how it is spatialised, an event instance owns none of that.
	// Distance curves, cones, reverb sends, randomisation and DSP all live in the event as the sound
	// designer authored it, and the engine's job shrinks to placing it in the world and driving the
	// parameters the designer exposed.
	//
	// That is the point of the migration: mixing decisions move out of C++ and into FMOD Studio,
	// where they can be changed without a rebuild - and, with live update on, without even stopping
	// the game.
	class AudioEventInstance : public RefCounted
	{
	public:
		~AudioEventInstance();

		// Resolves the event by GUID and creates an instance of it. Returns null when the event is
		// not in any loaded bank, which is the normal outcome for a scene saved against banks that
		// have not been built yet.
		static Ref<AudioEventInstance> Create(const std::string& eventGuid);

		void Start();
		void Stop(bool allowFadeOut = true);
		void SetPaused(bool paused);
		bool IsPlaying() const;

		// Position and orientation of the emitter in world space. Studio takes all four together.
		void Set3DAttributes(const glm::vec3& position, const glm::vec3& velocity,
			const glm::vec3& forward, const glm::vec3& up);

		void SetVolume(float volume);
		void SetPitch(float pitch);

		// Sets an author-defined continuous parameter. Returns false when the event has no parameter
		// by that name - which is not an error, just a project that has not authored one.
		bool SetParameter(const std::string& name, float value);

		// Feeds the ray-traced acoustics result into the event. Unlike the Core path, this does not
		// reach for a filter directly: it writes the two named parameters below, leaving the sound
		// designer to decide what occlusion actually *does* to this particular sound. An event that
		// declares neither is simply unaffected.
		void SetAcoustics(const AudioSourceAcoustics& acoustics);

		// The parameter names the engine writes acoustics into. Authoring an event with these names
		// is what opts it into ray-traced occlusion.
		static constexpr const char* kOcclusionParameter = "Occlusion";
		static constexpr const char* kReverbSendParameter = "ReverbSend";

	private:
		explicit AudioEventInstance(FMOD::Studio::EventInstance* instance)
			: m_Instance(instance) {}

		FMOD::Studio::EventInstance* m_Instance = nullptr;
	};

}
