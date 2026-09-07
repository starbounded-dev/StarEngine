#include "lpch.h"
#include "Lux/Audio/RaytracedAudioScene.h"

#ifdef LUX_ENABLE_RAYTRACED_AUDIO
	#include "vaudio.h"

	#include <algorithm>
	#include <cmath>
	#include <limits>
	#include <unordered_map>
#endif

namespace Lux {

#ifndef LUX_ENABLE_RAYTRACED_AUDIO

	// Built without Core/vendor/VA_RAY. Every entry point degrades to a no-op so call sites in
	// Scene stay free of #ifdef.

	bool RaytracedAudioScene::IsAvailable() { return false; }

	struct RaytracedAudioScene::Impl {};

	RaytracedAudioScene::RaytracedAudioScene(Scene* scene)
		: m_Scene(scene), m_Impl(CreateScope<Impl>())
	{
	}

	RaytracedAudioScene::~RaytracedAudioScene() = default;

	void RaytracedAudioScene::Start() {}
	void RaytracedAudioScene::Stop() {}
	void RaytracedAudioScene::OnUpdate(Timestep) {}
	void RaytracedAudioScene::SetStaticGeometry(const std::vector<glm::vec3>&) {}
	void RaytracedAudioScene::SetListener(const glm::vec3&, const glm::vec3&) {}
	void RaytracedAudioScene::CreateEmitter(UUID) {}
	void RaytracedAudioScene::DestroyEmitter(UUID) {}
	void RaytracedAudioScene::SetEmitterPosition(UUID, const glm::vec3&) {}
	RaytracedAudioResult RaytracedAudioScene::GetResult(UUID) const { return {}; }

#else

	namespace {

		constexpr VAMaterialType kDefaultStaticGeometryMaterial = VAMaterialConcrete;
		// Meters. Padded onto the mirrored geometry's AABB so emitters slightly above/beside it
		// (e.g. a bird sound over open terrain) are still inside the world and get raytraced.
		constexpr float kWorldBoundsPadding = 50.0f;
		// Meters. Used only until the first SetStaticGeometry() call gives us a real AABB.
		constexpr float kDefaultWorldHalfExtent = 500.0f;

		// The vendored libvaudionative.so exports only functions (EXPORT_API), not the header's
		// extern VAMatrix globals - VA_MATRIX_IDENTITY/VA_MATRIX_EMPTY link-fail on Linux. Identity
		// is the same under any row/column-major convention, so build it locally instead.
		constexpr VAMatrix kIdentityMatrix = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

		// A freshly created VAEmitter casts ZERO rays of every type - vaEmitterGetCastsAnyRays()
		// returns false until both a ray count and a bounce count are set, and an emitter that
		// casts nothing never produces an occlusion filter or reverb result. These are the values
		// from Vercidium's own getting-started example; they are the starting point for tuning
		// quality against CPU cost, not a hard requirement.
		constexpr int kReverbRayCount = 128;
		constexpr int kReverbBounceCount = 64;
		constexpr int kOcclusionRayCount = 512;
		constexpr int kOcclusionBounceCount = 8;
		constexpr int kPermeationRayCount = 128;
		constexpr int kPermeationBounceCount = 3;

		VAVector ToVA(const glm::vec3& v)
		{
			return vaVectorCreate(v.x, v.y, v.z);
		}

	}

	bool RaytracedAudioScene::IsAvailable() { return true; }

	struct RaytracedAudioScene::Impl
	{
		VAWorld* World = nullptr;
		VAEmitter* Listener = nullptr;
		std::unordered_map<UUID, VAEmitter*> SourceEmitters;
		std::vector<VAMeshPrimitive*> StaticPrimitives;
	};

	RaytracedAudioScene::RaytracedAudioScene(Scene* scene)
		: m_Scene(scene), m_Impl(CreateScope<Impl>())
	{
	}

	RaytracedAudioScene::~RaytracedAudioScene()
	{
		Stop();
	}

	void RaytracedAudioScene::Start()
	{
		LUX_PROFILE_FUNCTION_AUTO;

		if (m_Impl->World)
			return;

		m_Impl->World = vaWorldCreate();
		vaWorldSetPosition(m_Impl->World, vaVectorCreateUniform(-kDefaultWorldHalfExtent));
		vaWorldSetSize(m_Impl->World, vaVectorCreateUniform(kDefaultWorldHalfExtent * 2.0f));

		// The listener is a plain emitter used only as an occlusion/reverb target - it never casts
		// its own rays.
		m_Impl->Listener = vaEmitterCreate();
		vaEmitterSetHasRelativeReverb(m_Impl->Listener, true);
		vaEmitterSetReverbRayCount(m_Impl->Listener, 0);
		vaEmitterSetOcclusionRayCount(m_Impl->Listener, 0);
		vaEmitterSetPermeationRayCount(m_Impl->Listener, 0);
		vaEmitterSetAmbientOcclusionRayCount(m_Impl->Listener, 0);
		vaEmitterSetAmbientPermeationRayCount(m_Impl->Listener, 0);
		if (VAResult result = vaWorldAddEmitter(m_Impl->World, m_Impl->Listener); result != VA_SUCCESS)
			LUX_CORE_ERROR_TAG("Audio", "RaytracedAudioScene::Start failed to add the listener emitter to the world; no source will produce occlusion (VAResult={0})", result);
	}

	void RaytracedAudioScene::Stop()
	{
		LUX_PROFILE_FUNCTION_AUTO;

		if (!m_Impl->World)
			return;

		// Stop the background raytracing threads submitting new work before tearing anything down.
		vaWorldSetPendingShutdown(m_Impl->World, true);

		// vaWorldRemoveEmitter can answer VA_PENDING_REMOVAL: the world keeps the emitter alive
		// until its reverb tail finishes. Destroying it here regardless leaves the world holding a
		// freed pointer, which it frees again from vaWorldDestroy below - a double free. Only
		// destroy what actually came back out; vaWorldDestroy releases whatever the world still
		// owns along with itself.
		for (auto& [entityID, emitter] : m_Impl->SourceEmitters)
		{
			if (vaWorldRemoveEmitter(m_Impl->World, emitter) == VA_SUCCESS)
				vaEmitterDestroy(emitter);
		}
		m_Impl->SourceEmitters.clear();

		for (VAMeshPrimitive* primitive : m_Impl->StaticPrimitives)
		{
			if (vaWorldRemovePrimitive_(m_Impl->World, primitive) == VA_SUCCESS)
				vaMeshPrimitiveDestroy(primitive);
		}
		m_Impl->StaticPrimitives.clear();

		if (m_Impl->Listener)
		{
			if (vaWorldRemoveEmitter(m_Impl->World, m_Impl->Listener) == VA_SUCCESS)
				vaEmitterDestroy(m_Impl->Listener);
			m_Impl->Listener = nullptr;
		}

		// Waits for the background threads, then frees the world and anything still attached.
		vaWorldDestroy(m_Impl->World);
		m_Impl->World = nullptr;
	}

	void RaytracedAudioScene::OnUpdate(Timestep ts)
	{
		LUX_PROFILE_FUNCTION_AUTO;
		(void)ts;

		if (!m_Impl->World)
			return;

		vaWorldUpdate(m_Impl->World);
	}

	void RaytracedAudioScene::SetStaticGeometry(const std::vector<glm::vec3>& worldSpaceTriangles)
	{
		LUX_PROFILE_FUNCTION_AUTO;

		if (!m_Impl->World)
			return;

		for (VAMeshPrimitive* primitive : m_Impl->StaticPrimitives)
		{
			if (vaWorldRemovePrimitive_(m_Impl->World, primitive) == VA_SUCCESS)
				vaMeshPrimitiveDestroy(primitive);
		}
		m_Impl->StaticPrimitives.clear();

		if (worldSpaceTriangles.empty())
			return;

		if (worldSpaceTriangles.size() % 3 != 0)
		{
			LUX_CORE_ERROR_TAG("Audio", "RaytracedAudioScene::SetStaticGeometry received {0} vertices, which is not a multiple of 3 - ignoring", worldSpaceTriangles.size());
			return;
		}

		glm::vec3 minBounds(std::numeric_limits<float>::max());
		glm::vec3 maxBounds(std::numeric_limits<float>::lowest());
		std::vector<VAVector> vertices;
		vertices.reserve(worldSpaceTriangles.size());
		for (const glm::vec3& v : worldSpaceTriangles)
		{
			vertices.push_back(ToVA(v));
			minBounds = glm::min(minBounds, v);
			maxBounds = glm::max(maxBounds, v);
		}

		VAMeshPrimitive* primitive = nullptr;
		VAResult result = vaMeshPrimitiveCreate(kDefaultStaticGeometryMaterial, vertices.data(), (int)vertices.size(), ToVA(minBounds), ToVA(maxBounds), &kIdentityMatrix, &primitive);
		if (result != VA_SUCCESS || !primitive)
		{
			LUX_CORE_ERROR_TAG("Audio", "RaytracedAudioScene::SetStaticGeometry failed to create the mirrored mesh primitive (VAResult={0})", result);
			return;
		}

		if (VAResult addResult = vaWorldAddPrimitive_(m_Impl->World, primitive); addResult != VA_SUCCESS)
		{
			LUX_CORE_ERROR_TAG("Audio", "RaytracedAudioScene::SetStaticGeometry failed to add the mirrored geometry to the world; occlusion will not work (VAResult={0})", addResult);
			vaMeshPrimitiveDestroy(primitive);
			return;
		}

		m_Impl->StaticPrimitives.push_back(primitive);

		const glm::vec3 paddedMin = minBounds - kWorldBoundsPadding;
		const glm::vec3 paddedMax = maxBounds + kWorldBoundsPadding;
		vaWorldSetPosition(m_Impl->World, ToVA(paddedMin));
		vaWorldSetSize(m_Impl->World, ToVA(paddedMax - paddedMin));
	}

	void RaytracedAudioScene::SetListener(const glm::vec3& position, const glm::vec3& forward)
	{
		if (!m_Impl->World || !m_Impl->Listener)
			return;

		vaEmitterSetPosition(m_Impl->Listener, ToVA(position));

		const glm::vec3 dir = glm::normalize(forward);
		const float pitch = std::asin(std::clamp(dir.y, -1.0f, 1.0f));
		const float yaw = std::atan2(-dir.x, -dir.z);

		vaWorldSetCameraPosition(m_Impl->World, ToVA(position));
		vaWorldSetCameraPitch(m_Impl->World, pitch);
		vaWorldSetCameraYaw(m_Impl->World, yaw);
	}

	void RaytracedAudioScene::CreateEmitter(UUID entityID)
	{
		if (!m_Impl->World || m_Impl->SourceEmitters.contains(entityID))
			return;

		VAEmitter* emitter = vaEmitterCreate();

		// Without these the emitter casts no rays at all and never yields a result (see the
		// constants above).
		vaEmitterSetReverbRayCount(emitter, kReverbRayCount);
		vaEmitterSetReverbBounceCount(emitter, kReverbBounceCount);
		vaEmitterSetOcclusionRayCount(emitter, kOcclusionRayCount);
		vaEmitterSetOcclusionBounceCount(emitter, kOcclusionBounceCount);
		vaEmitterSetPermeationRayCount(emitter, kPermeationRayCount);
		vaEmitterSetPermeationBounceCount(emitter, kPermeationBounceCount);

		if (VAResult result = vaWorldAddEmitter(m_Impl->World, emitter); result != VA_SUCCESS)
		{
			LUX_CORE_ERROR_TAG("Audio", "RaytracedAudioScene::CreateEmitter failed to add emitter for entity {0} to the world (VAResult={1})", (uint64_t)entityID, result);
			vaEmitterDestroy(emitter);
			return;
		}

		if (m_Impl->Listener)
		{
			if (VAResult result = vaEmitterAddTarget(emitter, m_Impl->Listener); result != VA_SUCCESS)
				LUX_CORE_ERROR_TAG("Audio", "RaytracedAudioScene::CreateEmitter failed to target the listener from entity {0}; it will produce no occlusion (VAResult={1})", (uint64_t)entityID, result);
		}

		m_Impl->SourceEmitters[entityID] = emitter;
	}

	void RaytracedAudioScene::DestroyEmitter(UUID entityID)
	{
		auto it = m_Impl->SourceEmitters.find(entityID);
		if (it == m_Impl->SourceEmitters.end())
			return;

		// As in Stop(): a VA_PENDING_REMOVAL emitter is still owned by the world (its reverb tail
		// is still playing), so dropping our handle is all we may safely do - the world frees it.
		if (m_Impl->World && vaWorldRemoveEmitter(m_Impl->World, it->second) == VA_SUCCESS)
			vaEmitterDestroy(it->second);

		m_Impl->SourceEmitters.erase(it);
	}

	void RaytracedAudioScene::SetEmitterPosition(UUID entityID, const glm::vec3& position)
	{
		auto it = m_Impl->SourceEmitters.find(entityID);
		if (it == m_Impl->SourceEmitters.end())
			return;

		vaEmitterSetPosition(it->second, ToVA(position));
	}

	RaytracedAudioResult RaytracedAudioScene::GetResult(UUID entityID) const
	{
		RaytracedAudioResult result;

		auto it = m_Impl->SourceEmitters.find(entityID);
		if (it == m_Impl->SourceEmitters.end() || !m_Impl->Listener)
			return result;

		VAEmitter* emitter = it->second;

		// An emitter added to the world this frame has not been through a vaWorldUpdate yet, so
		// its result buffers do not exist and the accessors below dereference them regardless -
		// querying one that is still initialising segfaults inside the SDK rather than returning
		// NULL. Scene::OnUpdateRuntime creates emitters and reads results in the same pass, so
		// this is hit on the first frame of every newly-added source.
		if (vaEmitterGetInitialising(emitter) || vaEmitterGetPendingRemoval(emitter))
			return result;

		if (const VALowPassFilter* filter = vaEmitterGetTargetFilter(emitter, m_Impl->Listener))
		{
			result.OcclusionGainLF = filter->gainLF;
			result.OcclusionGainHF = filter->gainHF;
			result.Valid = true;
		}

		if (const VAProcessedReverb* reverb = vaEmitterGetProcessedReverb(emitter))
		{
			result.ReverbReturnedPercent = reverb->returnedPercent;
			result.ReverbOutsidePercent = reverb->outsidePercent;
			result.ReverbDecayTimeLF = reverb->measuredDecayTimeLF;
			result.ReverbDecayTimeHF = reverb->measuredDecayTimeHF;
			result.Valid = true;
		}

		return result;
	}

#endif

}
