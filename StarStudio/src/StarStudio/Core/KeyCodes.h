#pragma once

namespace StarStudio
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define SS_KEY_SPACE           ::StarStudio::Key::Space
#define SS_KEY_APOSTROPHE      ::StarStudio::Key::Apostrophe    /* ' */
#define SS_KEY_COMMA           ::StarStudio::Key::Comma         /* , */
#define SS_KEY_MINUS           ::StarStudio::Key::Minus         /* - */
#define SS_KEY_PERIOD          ::StarStudio::Key::Period        /* . */
#define SS_KEY_SLASH           ::StarStudio::Key::Slash         /* / */
#define SS_KEY_0               ::StarStudio::Key::D0
#define SS_KEY_1               ::StarStudio::Key::D1
#define SS_KEY_2               ::StarStudio::Key::D2
#define SS_KEY_3               ::StarStudio::Key::D3
#define SS_KEY_4               ::StarStudio::Key::D4
#define SS_KEY_5               ::StarStudio::Key::D5
#define SS_KEY_6               ::StarStudio::Key::D6
#define SS_KEY_7               ::StarStudio::Key::D7
#define SS_KEY_8               ::StarStudio::Key::D8
#define SS_KEY_9               ::StarStudio::Key::D9
#define SS_KEY_SEMICOLON       ::StarStudio::Key::Semicolon     /* ; */
#define SS_KEY_EQUAL           ::StarStudio::Key::Equal         /* = */
#define SS_KEY_A               ::StarStudio::Key::A
#define SS_KEY_B               ::StarStudio::Key::B
#define SS_KEY_C               ::StarStudio::Key::C
#define SS_KEY_D               ::StarStudio::Key::D
#define SS_KEY_E               ::StarStudio::Key::E
#define SS_KEY_F               ::StarStudio::Key::F
#define SS_KEY_G               ::StarStudio::Key::G
#define SS_KEY_H               ::StarStudio::Key::H
#define SS_KEY_I               ::StarStudio::Key::I
#define SS_KEY_J               ::StarStudio::Key::J
#define SS_KEY_K               ::StarStudio::Key::K
#define SS_KEY_L               ::StarStudio::Key::L
#define SS_KEY_M               ::StarStudio::Key::M
#define SS_KEY_N               ::StarStudio::Key::N
#define SS_KEY_O               ::StarStudio::Key::O
#define SS_KEY_P               ::StarStudio::Key::P
#define SS_KEY_Q               ::StarStudio::Key::Q
#define SS_KEY_R               ::StarStudio::Key::R
#define SS_KEY_S               ::StarStudio::Key::S
#define SS_KEY_T               ::StarStudio::Key::T
#define SS_KEY_U               ::StarStudio::Key::U
#define SS_KEY_V               ::StarStudio::Key::V
#define SS_KEY_W               ::StarStudio::Key::W
#define SS_KEY_X               ::StarStudio::Key::X
#define SS_KEY_Y               ::StarStudio::Key::Y
#define SS_KEY_Z               ::StarStudio::Key::Z
#define SS_KEY_LEFT_BRACKET    ::StarStudio::Key::LeftBracket   /* [ */
#define SS_KEY_BACKSLASH       ::StarStudio::Key::Backslash     /* \ */
#define SS_KEY_RIGHT_BRACKET   ::StarStudio::Key::RightBracket  /* ] */
#define SS_KEY_GRAVE_ACCENT    ::StarStudio::Key::GraveAccent   /* ` */
#define SS_KEY_WORLD_1         ::StarStudio::Key::World1        /* non-US #1 */
#define SS_KEY_WORLD_2         ::StarStudio::Key::World2        /* non-US #2 */

/* Function keys */
#define SS_KEY_ESCAPE          ::StarStudio::Key::Escape
#define SS_KEY_ENTER           ::StarStudio::Key::Enter
#define SS_KEY_TAB             ::StarStudio::Key::Tab
#define SS_KEY_BACKSPACE       ::StarStudio::Key::Backspace
#define SS_KEY_INSERT          ::StarStudio::Key::Insert
#define SS_KEY_DELETE          ::StarStudio::Key::Delete
#define SS_KEY_RIGHT           ::StarStudio::Key::Right
#define SS_KEY_LEFT            ::StarStudio::Key::Left
#define SS_KEY_DOWN            ::StarStudio::Key::Down
#define SS_KEY_UP              ::StarStudio::Key::Up
#define SS_KEY_PAGE_UP         ::StarStudio::Key::PageUp
#define SS_KEY_PAGE_DOWN       ::StarStudio::Key::PageDown
#define SS_KEY_HOME            ::StarStudio::Key::Home
#define SS_KEY_END             ::StarStudio::Key::End
#define SS_KEY_CAPS_LOCK       ::StarStudio::Key::CapsLock
#define SS_KEY_SCROLL_LOCK     ::StarStudio::Key::ScrollLock
#define SS_KEY_NUM_LOCK        ::StarStudio::Key::NumLock
#define SS_KEY_PRINT_SCREEN    ::StarStudio::Key::PrintScreen
#define SS_KEY_PAUSE           ::StarStudio::Key::Pause
#define SS_KEY_F1              ::StarStudio::Key::F1
#define SS_KEY_F2              ::StarStudio::Key::F2
#define SS_KEY_F3              ::StarStudio::Key::F3
#define SS_KEY_F4              ::StarStudio::Key::F4
#define SS_KEY_F5              ::StarStudio::Key::F5
#define SS_KEY_F6              ::StarStudio::Key::F6
#define SS_KEY_F7              ::StarStudio::Key::F7
#define SS_KEY_F8              ::StarStudio::Key::F8
#define SS_KEY_F9              ::StarStudio::Key::F9
#define SS_KEY_F10             ::StarStudio::Key::F10
#define SS_KEY_F11             ::StarStudio::Key::F11
#define SS_KEY_F12             ::StarStudio::Key::F12
#define SS_KEY_F13             ::StarStudio::Key::F13
#define SS_KEY_F14             ::StarStudio::Key::F14
#define SS_KEY_F15             ::StarStudio::Key::F15
#define SS_KEY_F16             ::StarStudio::Key::F16
#define SS_KEY_F17             ::StarStudio::Key::F17
#define SS_KEY_F18             ::StarStudio::Key::F18
#define SS_KEY_F19             ::StarStudio::Key::F19
#define SS_KEY_F20             ::StarStudio::Key::F20
#define SS_KEY_F21             ::StarStudio::Key::F21
#define SS_KEY_F22             ::StarStudio::Key::F22
#define SS_KEY_F23             ::StarStudio::Key::F23
#define SS_KEY_F24             ::StarStudio::Key::F24
#define SS_KEY_F25             ::StarStudio::Key::F25

/* Keypad */
#define SS_KEY_KP_0            ::StarStudio::Key::KP0
#define SS_KEY_KP_1            ::StarStudio::Key::KP1
#define SS_KEY_KP_2            ::StarStudio::Key::KP2
#define SS_KEY_KP_3            ::StarStudio::Key::KP3
#define SS_KEY_KP_4            ::StarStudio::Key::KP4
#define SS_KEY_KP_5            ::StarStudio::Key::KP5
#define SS_KEY_KP_6            ::StarStudio::Key::KP6
#define SS_KEY_KP_7            ::StarStudio::Key::KP7
#define SS_KEY_KP_8            ::StarStudio::Key::KP8
#define SS_KEY_KP_9            ::StarStudio::Key::KP9
#define SS_KEY_KP_DECIMAL      ::StarStudio::Key::KPDecimal
#define SS_KEY_KP_DIVIDE       ::StarStudio::Key::KPDivide
#define SS_KEY_KP_MULTIPLY     ::StarStudio::Key::KPMultiply
#define SS_KEY_KP_SUBTRACT     ::StarStudio::Key::KPSubtract
#define SS_KEY_KP_ADD          ::StarStudio::Key::KPAdd
#define SS_KEY_KP_ENTER        ::StarStudio::Key::KPEnter
#define SS_KEY_KP_EQUAL        ::StarStudio::Key::KPEqual

#define SS_KEY_LEFT_SHIFT      ::StarStudio::Key::LeftShift
#define SS_KEY_LEFT_CONTROL    ::StarStudio::Key::LeftControl
#define SS_KEY_LEFT_ALT        ::StarStudio::Key::LeftAlt
#define SS_KEY_LEFT_SUPER      ::StarStudio::Key::LeftSuper
#define SS_KEY_RIGHT_SHIFT     ::StarStudio::Key::RightShift
#define SS_KEY_RIGHT_CONTROL   ::StarStudio::Key::RightControl
#define SS_KEY_RIGHT_ALT       ::StarStudio::Key::RightAlt
#define SS_KEY_RIGHT_SUPER     ::StarStudio::Key::RightSuper
#define SS_KEY_MENU            ::StarStudio::Key::Menu