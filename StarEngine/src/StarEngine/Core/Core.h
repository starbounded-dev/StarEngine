#pragma once

#include "StarEngine/Core/PlatformDetection.h"

#include <memory>

#if defined(SE_PLATFORM_WINDOWS)
#define SE_DEBUGBREAK() __debugbreak()
#elif defined(SE_PLATFORM_LINUX)
#include <signal.h>
#define SE_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif

#ifdef SE_DEBUG
#define SE_ENABLE_ASSERTS
#endif

#ifndef SE_DIST
#define SE_ENABLE_VERIFY
#endif

#define SE_EXPAND_MACRO(x) x
#define SE_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define SE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace StarEngine {

	template<typename T>
	constexpr T AlignUp2(T value, uint64_t align)
	{
		return reinterpret_cast<T>((reinterpret_cast<uint64_t>(value) + align - 1) & ~(align - 1));
	}

	template<typename T>
	struct ScopeExit
	{
		T Func;

		ScopeExit(T&& func) noexcept
			: Func(std::move(func))
		{
		}

		ScopeExit(const ScopeExit&) = delete;
		ScopeExit& operator=(const ScopeExit&) = delete;
		ScopeExit(ScopeExit&&) noexcept = delete;
		ScopeExit& operator=(ScopeExit&&) noexcept = delete;

		~ScopeExit()
		{
			Func();
		}
	};

	using byte = uint8_t;

	//template<typename From, typename To>
	//concept CastableTo = requires { static_cast<To>(std::declval<From>()); };

	using float32_t = float;
	using float64_t = double;

	constexpr float32_t operator""_f32(long double value)
	{
		return static_cast<float32_t>(value);
	}

	constexpr float64_t operator""_f64(long double value)
	{
		return static_cast<float64_t>(value);
	}
}

#include "StarEngine/Core/Log.h"
#include "StarEngine/Core/Assert.h"
