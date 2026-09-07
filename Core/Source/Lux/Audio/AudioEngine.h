#pragma once

#ifdef LUX_ENABLE_FMOD
namespace FMOD { class System; }
#else
struct ma_engine;
#endif

namespace Lux {

	class AudioEngine
	{
	public:
		static void Init();
		static void Shutdown();

		// Pumped once per frame from Application::Run. Required by FMOD (System::update());
		// a no-op under miniaudio, which mixes on its own thread.
		static void Update();

#ifdef LUX_ENABLE_FMOD
		static FMOD::System* GetEngine() { return s_Engine; }
#else
		static ma_engine* GetEngine() { return s_Engine; }
#endif
		static bool ShuttingDownEngine() { return s_ShuttingDown; }

		static bool HasInitializedEngine() { return s_HasInitializedAudioEngine; }
		static void SetInitalizedEngine(bool value) { s_HasInitializedAudioEngine = value; }

	private:
#ifdef LUX_ENABLE_FMOD
		static FMOD::System* s_Engine;
#else
		static ma_engine* s_Engine;
#endif
		inline static bool s_HasInitializedAudioEngine = false;
		inline static bool s_ShuttingDown = false;
	};
}
