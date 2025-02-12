#pragma once

namespace StarEngine
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define SE_MOUSE_BUTTON_0      ::StarEngine::Mouse::Button0
#define SE_MOUSE_BUTTON_1      ::StarEngine::Mouse::Button1
#define SE_MOUSE_BUTTON_2      ::StarEngine::Mouse::Button2
#define SE_MOUSE_BUTTON_3      ::StarEngine::Mouse::Button3
#define SE_MOUSE_BUTTON_4      ::StarEngine::Mouse::Button4
#define SE_MOUSE_BUTTON_5      ::StarEngine::Mouse::Button5
#define SE_MOUSE_BUTTON_6      ::StarEngine::Mouse::Button6
#define SE_MOUSE_BUTTON_7      ::StarEngine::Mouse::Button7
#define SE_MOUSE_BUTTON_LAST   ::StarEngine::Mouse::ButtonLast
#define SE_MOUSE_BUTTON_LEFT   ::StarEngine::Mouse::ButtonLeft
#define SE_MOUSE_BUTTON_RIGHT  ::StarEngine::Mouse::ButtonRight
#define SE_MOUSE_BUTTON_MIDDLE ::StarEngine::Mouse::ButtonMiddle