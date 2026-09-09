#pragma once

#include "Lux/Audio/RaytracedAudioScene.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#ifdef LUX_ENABLE_FMOD
namespace FMOD {
	class System;
	namespace Studio { class System; }
}
#else
struct ma_engine;
#endif

namespace Lux {

	// Backend-agnostic snapshot of the playback engine for editor tooling. LUX_ENABLE_FMOD is a
	// Core-only define (see Core/premake5.lua), so consumers outside Core cannot branch on the
	// backend themselves - the backend identifies itself here as data instead.
	struct AudioEngineStats
	{
		const char* BackendName = "None";
		bool Initialized = false;

		// Version of the backend itself, when it reports one (FMOD). Zero otherwise.
		uint32_t VersionMajor = 0;
		uint32_t VersionMinor = 0;
		uint32_t VersionPatch = 0;

		int SampleRate = 0;

		// False under miniaudio, which exposes no voice-count or CPU-usage equivalent; the fields
		// below stay zero and the editor labels them unavailable rather than showing a false zero.
		bool HasMixerStats = false;
		int ChannelsPlaying = 0;
		int RealChannelsPlaying = 0;
		float DSPCPUPercent = 0.0f;
		float StreamCPUPercent = 0.0f;
		float UpdateCPUPercent = 0.0f;
		int MemoryCurrentBytes = 0;
		int MemoryPeakBytes = 0;

		// FMOD Studio layer. Zeroed under any backend that has no bank concept.
		bool LiveUpdateEnabled = false;
		int LoadedBankCount = 0;
		int EventDescriptionCount = 0;
		int PlayingEventInstances = 0;
	};

	// A bank loaded from the project's built bank directory. Banks are build output of the FMOD
	// Studio project - see AudioBankBuilder.
	struct AudioBankInfo
	{
		std::string Name;              // file name as it sits on disk, e.g. "Master.bank"
		int EventCount = 0;

		// The strings bank carries the event *path* table. Without it events can only be resolved
		// by GUID, so its absence is worth surfacing rather than discovering through a failed
		// lookup.
		bool IsStringsBank = false;
	};

	// One event the loaded banks describe. Enumerated from the banks themselves rather than from
	// the GUIDs.txt that fmodstudiocl exports, so the list can never disagree with what is actually
	// loaded.
	struct AudioEventInfo
	{
		std::string Path;        // "event:/FX/Door"
		std::string Guid;        // "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" - the stable reference
		bool Is3D = false;
		bool IsOneshot = false;
	};

	class AudioEngine
	{
	public:
		static void Init();
		static void Shutdown();

		// Queried per frame by the editor's Audio Debugger. Cheap on both backends (plain getters);
		// safe to call before Init or after Shutdown, where it reports Initialized = false.
		static AudioEngineStats GetStats();

		// Applies the ray-traced reverb of the space the listener is in to the ambient reverb unit,
		// converting the SDK's units (seconds, Hz, linear gain) to the backend's. Driven once per
		// frame from Scene::OnUpdateRuntime; individual sources control only how much they send
		// into it, via AudioSource::SetReverbSend.
		//
		// A no-op under miniaudio, which has no reverb unit to drive.
		static void SetReverb(const RaytracedAudioReverb& reverb);

		// The reverb parameters most recently pushed to the backend, in that backend's own units,
		// for the editor to display next to the simulation's output. Zeroed if nothing was applied.
		struct ReverbSnapshot
		{
			bool Applied = false;
			float DecayTimeMs = 0.0f;
			float EarlyDelayMs = 0.0f;
			float LateDelayMs = 0.0f;
			float HFReferenceHz = 0.0f;
			float HFDecayRatioPercent = 0.0f;
			float DiffusionPercent = 0.0f;
			float DensityPercent = 0.0f;
			float LowShelfFrequencyHz = 0.0f;
			float LowShelfGainDb = 0.0f;
			float HighCutHz = 0.0f;
			float EarlyLateMixPercent = 0.0f;
			float WetLevelDb = 0.0f;
		};
		static ReverbSnapshot GetReverbSnapshot();

		// Pumped once per frame from Application::Run. Required by FMOD (Studio::System::update(),
		// which also pumps the core system); a no-op under miniaudio, which mixes on its own thread.
		static void Update();

		// --- FMOD Studio banks and events -------------------------------------------------------
		//
		// Banks are what the engine actually consumes: the .fspro is authored in the FMOD Studio
		// app and AudioBankBuilder turns it into these. Loading replaces whatever was loaded before,
		// so this is safe to call again after a rebuild.
		//
		// The strings bank is loaded first and deliberately: it carries the path table, and without
		// it every "event:/..." lookup fails with a not-found that does not explain itself.
		static bool LoadBanks(const std::filesystem::path& bankDirectory);
		static void UnloadAllBanks();
		static const std::vector<AudioBankInfo>& GetLoadedBanks();

		// Every event the loaded banks describe, sorted by path. Rebuilt by LoadBanks; empty when
		// no banks are loaded.
		static const std::vector<AudioEventInfo>& GetEvents();

		// --- Mixer buses ------------------------------------------------------------------------
		//
		// The game-facing volume controls. busPath is an FMOD bus path ("bus:/", "bus:/SFX"), which
		// the sound designer defines in Studio - the engine does not invent the hierarchy, it only
		// drives what is authored. Volume is linear, 0 to 1. Both return false / 0 when the bus does
		// not exist, which is the normal answer for a project that has not authored that bus.
		static bool SetBusVolume(const std::string& busPath, float volume);
		static float GetBusVolume(const std::string& busPath);

#ifdef LUX_ENABLE_FMOD
		// The Core system. Under Studio this is not created directly - Studio::System::initialize
		// creates it, and this is the handle it hands back. Still the right object for the low-level
		// work Studio does not cover: the 3D listener, the ray-traced reverb unit, and CPU stats.
		static FMOD::System* GetEngine() { return s_Engine; }

		// The Studio system, which owns banks, events and buses. Null when FMOD failed to
		// initialize.
		static FMOD::Studio::System* GetStudioSystem() { return s_StudioSystem; }
#else
		static ma_engine* GetEngine() { return s_Engine; }
#endif
		static bool ShuttingDownEngine() { return s_ShuttingDown; }

		static bool HasInitializedEngine() { return s_HasInitializedAudioEngine; }
		static void SetInitalizedEngine(bool value) { s_HasInitializedAudioEngine = value; }

	private:
#ifdef LUX_ENABLE_FMOD
		static FMOD::System* s_Engine;
		static FMOD::Studio::System* s_StudioSystem;
#else
		static ma_engine* s_Engine;
#endif
		inline static bool s_HasInitializedAudioEngine = false;
		inline static bool s_ShuttingDown = false;
	};
}
