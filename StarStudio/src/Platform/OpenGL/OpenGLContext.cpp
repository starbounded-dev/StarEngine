#include "sspch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace StarStudio {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		SS_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SS_CORE_ASSERT(status, "Failed to initialize Glad!");

		SS_CORE_INFO("OpenGL Info:");
		SS_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		SS_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		SS_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}