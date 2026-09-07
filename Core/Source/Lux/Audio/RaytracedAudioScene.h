#pragma once

#include "Lux/Core/Ref.h"
#include "Lux/Core/Timestep.h"
#include "Lux/Core/UUID.h"

#include <vector>

namespace Lux {

	class Scene;

	// Per-source result of the ray-traced acoustic simulation, meant to drive a playback backend's
	// per-voice low-pass filter and reverb send. Stays at "no effect" defaults until raytracing has
	// produced at least one result for that source.
	struct RaytracedAudioResult
	{
		bool Valid = false;

		// Direct-path occlusion toward the listener, as two-band linear gains (1 = unoccluded).
		float OcclusionGainLF = 1.0f;
		float OcclusionGainHF = 1.0f;

		// Reverb send: how much of the source's energy returns from the environment (vs. escaping
		// outside it) and how long it takes to decay, split into low/high frequency bands.
		float ReverbReturnedPercent = 0.0f;
		float ReverbOutsidePercent = 0.0f;
		float ReverbDecayTimeLF = 0.0f;
		float ReverbDecayTimeHF = 0.0f;
	};

	// Owns the ray-traced acoustics simulation (Vercidium Audio, Core/vendor/VA_RAY) for one Scene:
	// mirrors static geometry, tracks one listener and one emitter per audio-emitting entity, and
	// exposes per-emitter occlusion/reverb results for a playback backend to apply.
	//
	// Built without Core/vendor/VA_RAY present (LUX_ENABLE_RAYTRACED_AUDIO undefined), every method
	// is a no-op and GetResult never returns a valid result, so call sites need no #ifdef.
	class RaytracedAudioScene : public RefCounted
	{
	public:
		// True when Core/vendor/VA_RAY was compiled in (LUX_ENABLE_RAYTRACED_AUDIO). Independent of
		// whether Start() has run. Scene checks this before constructing a RaytracedAudioScene at
		// all, so a scene running without the feature compiled in pays no per-frame cost for it.
		static bool IsAvailable();

		explicit RaytracedAudioScene(Scene* scene);
		~RaytracedAudioScene();

		void Start();
		void Stop();
		void OnUpdate(Timestep ts);

		// Rebuilds the mirrored static geometry from a flat, world-space triangle soup (3 positions
		// per triangle). Replaces whatever geometry was previously mirrored.
		void SetStaticGeometry(const std::vector<glm::vec3>& worldSpaceTriangles);

		void SetListener(const glm::vec3& position, const glm::vec3& forward);

		void CreateEmitter(UUID entityID);
		void DestroyEmitter(UUID entityID);
		void SetEmitterPosition(UUID entityID, const glm::vec3& position);

		RaytracedAudioResult GetResult(UUID entityID) const;

	private:
		Scene* m_Scene = nullptr;

		struct Impl;
		Scope<Impl> m_Impl;
	};

}
