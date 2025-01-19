#pragma once

#ifdef SS_PLATFORM_WINDOWS
	#ifdef SS_BUILD_DLL
		#define STARSTUDIO_API __declspec(dllexport)
	#else
		#define STARSTUDIO_API __declspec(dllimport)
	#endif
#else
	#error StarStudio only supports Windows!
#endif

#ifdef SS_ENABLE_ASSERTS
	#define SS_ASSERT(x, ...) { if(!(x)) { SS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define SS_CORE_ASSERT(x, ...) { if(!(x)) { SS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define SS_ASSERT(x, ...)
	#define SS_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1 << x)