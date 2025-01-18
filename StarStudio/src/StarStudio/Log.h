#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"

namespace StarStudio {
	class STARSTUDIO_API Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define SS_CORE_TRACE(...) ::StarStudio::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SS_CORE_INFO(...) ::StarStudio::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SS_CORE_WARN(...) ::StarStudio::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SS_CORE_ERROR(...) ::StarStudio::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SS_CORE_FATAL(...) ::StarStudio::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define SS_TRACE(...) ::StarStudio::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SS_INFO(...) ::StarStudio::Log::GetClientLogger()->info(__VA_ARGS__)
#define SS_WARN(...) ::StarStudio::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SS_ERROR(...) ::StarStudio::Log::GetClientLogger()->error(__VA_ARGS__)
#define SS_FATAL(...) ::StarStudio::Log::GetClientLogger()->fatal(__VA_ARGS__)


