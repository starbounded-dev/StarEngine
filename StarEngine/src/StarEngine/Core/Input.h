#pragma once

#include "StarEngine/Core/Base.h"
#include "StarEngine/Core/KeyCodes.h"
#include "StarEngine/Core/MouseCodes.h"

namespace StarEngine {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);

		static std::pair<float, float> GetMousePosition();

		static float GetMouseX();
		static float GetMouseY();
	};

}