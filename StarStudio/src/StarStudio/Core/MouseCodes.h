#pragma once

namespace StarStudio
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

#define SS_MOUSE_BUTTON_0      ::StarStudio::Mouse::Button0
#define SS_MOUSE_BUTTON_1      ::StarStudio::Mouse::Button1
#define SS_MOUSE_BUTTON_2      ::StarStudio::Mouse::Button2
#define SS_MOUSE_BUTTON_3      ::StarStudio::Mouse::Button3
#define SS_MOUSE_BUTTON_4      ::StarStudio::Mouse::Button4
#define SS_MOUSE_BUTTON_5      ::StarStudio::Mouse::Button5
#define SS_MOUSE_BUTTON_6      ::StarStudio::Mouse::Button6
#define SS_MOUSE_BUTTON_7      ::StarStudio::Mouse::Button7
#define SS_MOUSE_BUTTON_LAST   ::StarStudio::Mouse::ButtonLast
#define SS_MOUSE_BUTTON_LEFT   ::StarStudio::Mouse::ButtonLeft
#define SS_MOUSE_BUTTON_RIGHT  ::StarStudio::Mouse::ButtonRight
#define SS_MOUSE_BUTTON_MIDDLE ::StarStudio::Mouse::ButtonMiddle