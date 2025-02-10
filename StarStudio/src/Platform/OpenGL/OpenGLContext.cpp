#include "sspch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace StarStudio {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		SS_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		SS_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SS_CORE_ASSERT(status, "Failed to initialize Glad!");

		SS_CORE_INFO("OpenGL Info:");
		SS_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		SS_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		SS_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		#ifdef SS_ENABLE_ASSERTS
			int versionMajor;
			int versionMinor;
			glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
			glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
			SS_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "StarStudio requires at least OpenGL version 4.5!");
		#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		SS_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}