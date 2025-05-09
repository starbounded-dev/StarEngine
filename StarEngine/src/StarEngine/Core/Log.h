#pragma once

#include "StarEngine/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace StarEngine {
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

#define SE_CORE_TRACE(...) ::StarEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SE_CORE_INFO(...) ::StarEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SE_CORE_WARN(...) ::StarEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SE_CORE_ERROR(...) ::StarEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SE_CORE_CRITICAL(...) ::StarEngine::Log::GetClientLogger()->critical(__VA_ARGS__)

#define SE_TRACE(...) ::StarEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SE_INFO(...) ::StarEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define SE_WARN(...) ::StarEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SE_ERROR(...) ::StarEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define SE_CRITICAL(...) ::StarEngine::Log::GetClientLogger()->critical(__VA_ARGS__)
