#include "sspch.h"
#include "StarStudio/Core/Window.h"

#ifdef SS_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace StarStudio
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef SS_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		SS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}