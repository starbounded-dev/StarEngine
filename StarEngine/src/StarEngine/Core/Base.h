#pragma once

#include "StarEngine/Core/Ref.h"
#include <functional>
#include <memory>

namespace StarEngine
{
	void InitializeCore();
	void ShutdownCore();
};

#if defined(_WIN64) || defined(_WIN32)
		#define SE_PLATFORM_WINDOWS
#elif defined(__linux__)
	#define SE_PLATFORM_LINUX
	#define SE_PLATFORM_UNIX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	#define SE_PLATFORM_BSD
	#define SE_PLATFORM_UNIX
#elif defined(__unix__) || defined(__unix)
	#define SE_PLATFORM_UNIX
#else
	#error "Unsupported platform! StarEngine supports Windows, Linux, and BSD."
#endif

#define BIT(x) (1u << x)

//------------------------------------------------------------------------------
// Compiler Detection
//------------------------------------------------------------------------------

#if defined(__clang__)
	#define SE_COMPILER_CLANG
#elif defined(__GNUC__)
	#define SE_COMPILER_GCC
#elif defined(_MSC_VER)
	#define SE_COMPILER_MSVC
#else
	#error "Unknown compiler! StarEngine only supports MSVC, GCC, and Clang."
#endif

//------------------------------------------------------------------------------
// Function Inlining & Static Declaration
//------------------------------------------------------------------------------

#if defined(SE_COMPILER_MSVC)
	#define SE_FORCE_INLINE    __forceinline
	#define SE_EXPLICIT_STATIC static
#elif defined(SE_COMPILER_GCC) || defined(SE_COMPILER_CLANG)
	#define SE_FORCE_INLINE    __attribute__((always_inline)) inline
	#define SE_EXPLICIT_STATIC
#else
	#define SE_FORCE_INLINE    inline
	#define SE_EXPLICIT_STATIC
#endif

// Pointer wrappers
namespace StarEngine {

	template<typename T>
	T RoundDown(T x, T fac) { return x / fac * fac; }

	template<typename T>
	T RoundUp(T x, T fac) { return RoundDown(x + fac - 1, fac); }

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	using byte = uint8_t;

}
