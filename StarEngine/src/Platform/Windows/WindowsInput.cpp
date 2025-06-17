#include "sepch.h"
#include "StarEngine/Core/Input.h"
#include "StarEngine/Core/Application.h"

#include <GLFW/glfw3.h>

namespace StarEngine {

	bool Input::IsKeyPressed(const KeyCode key)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

	CursorMode Input::GetCursorMode()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int mode = glfwGetInputMode(window, GLFW_CURSOR);

		switch (mode)
		{
		case GLFW_CURSOR_NORMAL:
			return CursorMode::Normal;
		case GLFW_CURSOR_HIDDEN:
			return CursorMode::Hidden;
		case GLFW_CURSOR_DISABLED:
			return CursorMode::Disabled;
		default:
			return CursorMode::Normal; // Default fallback
		}
	}
}
