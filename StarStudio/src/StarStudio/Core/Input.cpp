#include "sspch.h"
#include "StarStudio/Core/Input.h"

#ifdef SS_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsInput.h"
#endif

namespace StarStudio {
	Scope<Input> Input::s_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
#ifdef SS_PLATFORM_WINDOWS
		return CreateScope<WindowsInput>();
#else
		SS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}