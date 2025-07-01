#pragma once

#define SE_ENABLE_PROFILING !SE_DIST

#if SE_ENABLE_PROFILING
#include <tracy/Tracy.hpp>
#endif

#if SE_ENABLE_PROFILING
	#define SE_PROFILE_MARK_FRAME			FrameMark;
	#define SE_PROFILE_FUNC(...)			ZoneScoped##__VA_OPT__(N(__VA_ARGS__))
	#define SE_PROFILE_SCOPE(...)			SE_PROFILE_FUNC(__VA_ARGS__)
	#define SE_PROFILE_SCOPE_DYNAMIC(NAME)  ZoneScoped; ZoneName(NAME, strlen(NAME))
	#define SE_PROFILE_THREAD(...)          tracy::SetThreadName(__VA_ARGS__)
#else
	#define SE_PROFILE_MARK_FRAME
	#define SE_PROFILE_FUNC(...)
	#define SE_PROFILE_SCOPE(...)
	#define SE_PROFILE_SCOPE_DYNAMIC(NAME)
	#define SE_PROFILE_THREAD(...)
#endif
