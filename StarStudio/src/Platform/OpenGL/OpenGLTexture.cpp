#include "sspch.h"
#include "OpenGLTexture.h"

#include "glad/glad.h"
#include "stb_image.h"

namespace StarStudio
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path), m_Width(0), m_Height(0)
	{
		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		SS_CORE_ASSERT(data, "Failed to load image!");
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{

	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{

	}
}

