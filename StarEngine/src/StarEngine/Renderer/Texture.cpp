#include "sepch.h"
#include "StarEngine/Renderer/Texture.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine
{
	namespace Utils {

		static nvrhi::SamplerAddressMode NVRHISamplerWrap(TextureWrap wrap)
		{
			switch (wrap)
			{
			case TextureWrap::Clamp: return nvrhi::SamplerAddressMode::Clamp;
			case TextureWrap::Repeat: return nvrhi::SamplerAddressMode::Repeat;
			}
			SE_CORE_ASSERT(false, "Unknown TextureWrap type");
			return (nvrhi::SamplerAddressMode)0; // Default to 0 if unknown
		}

		static bool NVRHISamplerWrap(TextureFilter filter)
		{
			switch (filter)
			{
			case TextureFilter::Linear:
			case TextureFilter::Cubic: return true;
			case TextureFilter::Nearest: return false;
			}

			SE_CORE_ASSERT(false, "Unknown TextureFilter type");
			return false;
		}

		static size_t GetMemoryType(ImageFormat format, uint32_t width, uint32_t height)
		{
			switch (format)
			{
			case ImageFormat::RED16UI: return width * height * sizeof(uint16_t);
			case ImageFormat::RG16F: return width * height * sizeof(uint16_t) * 2;
			case ImageFormat::RG32F: return width * height * sizeof(float) * 2;
			case ImageFormat::RED32F: return width * height * sizeof(float);
			case ImageFormat::RED8UI: return width * height;
			case ImageFormat::RED8UN: return width * height;
			case ImageFormat::RGBA: return width * height * 4;
			case ImageFormat::SRGBA: return width * height * 4;
			case ImageFormat::RGBA32F: return width * height * sizeof(float) * 4;
			case ImageFormat::B10R11G11F: return width * height * sizeof(float);
			}
			SE_CORE_ASSERT(false, "Unknown ImageFormat for memory type calculation!");
			return 0;
		}

		static bool ValidateSpecification(const TextureSpecification& specification)
		{
			bool result = true;

			result = specification.Width > 0 && specification.Height > 0 && specification.Width < 65536 && specification.Height < 65536;
			SE_CORE_VERIFY(result, "Texture width and height must be between 1 and 65535 pixels.");

			return result;
		}
	}

}
