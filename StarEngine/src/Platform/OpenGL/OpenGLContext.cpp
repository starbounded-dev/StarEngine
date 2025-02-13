#include "sepch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace StarEngine {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		SE_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		SE_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SE_CORE_ASSERT(status, "Failed to initialize Glad!");

		SE_CORE_INFO("OpenGL Info:");
		SE_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		SE_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		SE_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		#ifdef SS_ENABLE_ASSERTS
			int versionMajor;
			int versionMinor;
			glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
			glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
			SE_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "StarStudio requires at least OpenGL version 4.5!");
		#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		SE_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}