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

