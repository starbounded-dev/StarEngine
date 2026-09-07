#include "lpch.h"
#include "AudioEngine.h"
#include "AudioSource.h"

// AudioFileUtils.cpp reads file metadata via dr_wav (bundled in miniaudio.h) and stb_vorbis for
// the asset import pipeline, independent of which backend below actually plays audio - so these
// decoder implementations stay unconditional even when FMOD is the playback backend.
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c" // Enables Vorbis decoding.

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c" // Enables Vorbis decoding.

#ifdef LUX_ENABLE_FMOD
	#include <fmod.hpp>
	#include <fmod_errors.h>
#endif

namespace Lux {

#ifdef LUX_ENABLE_FMOD

	namespace {

		constexpr int kMaxChannels = 512;
		// One ambient, effectively unbounded reverb zone so per-source ReverbSend
		// (ChannelControl::setReverbProperties) has somewhere to go. This only carries the wet
		// send amount - per-source reverb character (decay time, roughness) from
		// RaytracedAudioScene isn't fed into FMOD's reverb properties yet.
		constexpr float kAmbientReverbMinDistance = 1.0f;
		constexpr float kAmbientReverbMaxDistance = 1000000.0f;
		FMOD::Reverb3D* s_AmbientReverb = nullptr;

	}

	FMOD::System* AudioEngine::s_Engine;

	void AudioEngine::Init()
	{
		LUX_PROFILE_FUNCTION("AudioEngine::Init");

		s_ShuttingDown = false;

		FMOD_RESULT result = FMOD::System_Create(&s_Engine);
		if (result != FMOD_OK)
		{
			LUX_CORE_ERROR("Failed to create FMOD system: {}", FMOD_ErrorString(result));
			s_Engine = nullptr;
			return;
		}

		result = s_Engine->init(kMaxChannels, FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_CHANNEL_LOWPASS, nullptr);
		if (result != FMOD_OK)
		{
			LUX_CORE_ERROR("Failed to initialize FMOD system: {}", FMOD_ErrorString(result));
			s_Engine->release();
			s_Engine = nullptr;
			return;
		}

		if (s_Engine->createReverb3D(&s_AmbientReverb) == FMOD_OK)
		{
			FMOD_VECTOR origin{ 0.0f, 0.0f, 0.0f };
			s_AmbientReverb->set3DAttributes(&origin, kAmbientReverbMinDistance, kAmbientReverbMaxDistance);
			FMOD_REVERB_PROPERTIES properties = FMOD_PRESET_GENERIC;
			s_AmbientReverb->setProperties(&properties);
		}
	}

	void AudioEngine::Shutdown()
	{
		LUX_PROFILE_FUNCTION("AudioEngine::Shutdown");

		s_ShuttingDown = true;

		if (s_AmbientReverb)
		{
			s_AmbientReverb->release();
			s_AmbientReverb = nullptr;
		}

		if (s_Engine)
		{
			s_Engine->close();
			s_Engine->release();
			s_Engine = nullptr;
		}
	}

	void AudioEngine::Update()
	{
		if (s_Engine)
			s_Engine->update();
	}

#else

	ma_engine* AudioEngine::s_Engine;

	void AudioEngine::Init()
	{
		LUX_PROFILE_FUNCTION("AudioEngine::Init");

		s_ShuttingDown = false;
		ma_engine_config engineConfig = ma_engine_config_init();
		engineConfig.listenerCount = 1;

		s_Engine = new ma_engine();
		ma_result result = ma_engine_init(&engineConfig, s_Engine);
		if (result != MA_SUCCESS)
		{
			ma_engine_uninit(s_Engine);
			delete s_Engine;

			LUX_CORE_ERROR("Failed to initialize audio engine!");
		}
	}

	void AudioEngine::Shutdown()
	{
		LUX_PROFILE_FUNCTION("AudioEngine::Shutdown");

		s_ShuttingDown = true;
		ma_engine_stop(s_Engine);
		ma_engine_uninit(s_Engine);
		delete s_Engine;
	}

	void AudioEngine::Update()
	{
		// miniaudio mixes on its own thread; nothing to pump here.
	}

#endif

}
