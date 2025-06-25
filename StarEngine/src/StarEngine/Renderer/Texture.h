#pragma once

#include "StarEngine/Core/Ref.h"
#include "StarEngine/Core/Buffer.h"

#include "StarEngine/Asset/Asset.h"
#include "StarEngine/Renderer/Image.h"

#include <nvrhi/nvrhi.h>

#include <filesystem>


namespace StarEngine {

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
