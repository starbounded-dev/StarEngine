#pragma once


#include "StarStudio/Core/Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace StarStudio {
	class Log
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

#define SS_CORE_TRACE(...) ::StarStudio::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SS_CORE_INFO(...) ::StarStudio::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SS_CORE_WARN(...) ::StarStudio::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SS_CORE_ERROR(...) ::StarStudio::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SS_CORE_CRITICAL(...) ::StarStudio::Log::GetClientLogger()->critical(__VA_ARGS__)

#define SS_TRACE(...) ::StarStudio::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SS_INFO(...) ::StarStudio::Log::GetClientLogger()->info(__VA_ARGS__)
#define SS_WARN(...) ::StarStudio::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SS_ERROR(...) ::StarStudio::Log::GetClientLogger()->error(__VA_ARGS__)
#define SS_CRITICAL(...) ::StarStudio::Log::GetClientLogger()->critical(__VA_ARGS__)