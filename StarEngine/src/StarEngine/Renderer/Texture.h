#pragma once

#include "StarEngine/Core/Ref.h"
#include "StarEngine/Core/Buffer.h"

#include "StarEngine/Asset/Asset.h"
#include "StarEngine/Renderer/Image.h"

#include <nvrhi/nvrhi.h>

#include <string>


namespace StarEngine {

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
				
			}
		}
	}

	struct TextureSpecification
	{
		ImageFormat Format = ImageFormat::RGBA;
		uint32_t Width = 1;
		uint32_t Height = 1;
		TextureWrap SamplerWrap = TextureWrap::Repeat;
		TextureFilter SamplerFilter = TextureFilter::Linear;

		bool GenerateMips = true;
		bool Storage = false;
		bool StoreLocally = false;

		std::string DebugName;
	};

	class Texture : public RendererResource
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual nvrhi::TextureHandle GetHandle() const = 0;

		virtual ImageFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual glm::uvec2 GetSize() const = 0;

		virtual uint32_t GetMipLevelCount() const = 0;
		virtual std::pair<uint32_t, uint32_t> GetMipSize(uint32_t mip) const = 0;

		virtual uint32_t GetHash() const = 0;
		
		virtual TextureType GetType() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static RefPtr<Texture2D> Create(const TextureSpecification& specification, Buffer data = Buffer());

		virtual void ChangeSize(uint32_t newWidth, uint32_t newHeight) = 0;

		static AssetType GetStaticType() { return AssetType::Texture2D; }
		virtual AssetType GetType() const { return GetStaticType(); }
	};
}
