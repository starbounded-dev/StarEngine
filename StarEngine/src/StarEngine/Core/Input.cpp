#include "sepch.h"
#include "StarEngine/Core/Input.h"

#ifdef SE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsInput.h"
#endif

namespace StarEngine {
	Scope<Input> Input::s_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
#ifdef SE_PLATFORM_WINDOWS
		return CreateScope<WindowsInput>();
#else
		SE_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}