#pragma once

#include "Base.h"

#define SE_VERSION "25.3.0.1"

#ifdef NDEBUG
		#define SE_RELEASE
#else
		#define SE_DEBUG
#endif

// ==== Build Configuration ====
#if defined(SE_DEBUG)
		#define SE_BUILD_CONFIG_NAME "Debug"
#elif defined(SE_RELEASE)
		#define SE_BUILD_CONFIG_NAME "Release"
#else
		#define SE_BUILD_CONFIG_NAME "Unknown"
#endif

// ==== Build Platform ====
#if defined(SE_PLATFORM_WINDOWS)
	#define SE_BUILD_PLATFORM_NAME "Windows x64"
#elif defined(SE_PLATFORM_LINUX)
	#define SE_BUILD_PLATFORM_NAME "Linux"
#elif defined(SE_PLATFORM_BSD)
	#define SE_BUILD_PLATFORM_NAME "BSD"
#elif defined(SE_PLATFORM_UNIX)
	#define SE_BUILD_PLATFORM_NAME "Unix"
#else
	#define SE_BUILD_PLATFORM_NAME "Unknown"
#endif

#define SE_VERSION_LONG "StarEngine " SE_VERSION " (" SE_BUILD_PLATFORM_NAME " " SE_BUILD_CONFIG_NAME ")"
