#pragma once

#include <filesystem>

#include "StarEngine/Core/Ref.h"
#include "StarEngine/Renderer/Texture.h"

namespace StarEngine {

	struct MSDFData;

	class Font : public Asset
	{
	public:
		Font(const std::filesystem::path& font);
		~Font();

		const MSDFData* GetMSDFData() const { return m_Data; }
		RefPtr<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

		static RefPtr<Font> GetDefault();
	private:
		MSDFData* m_Data;
		RefPtr<Texture2D> m_AtlasTexture;
	};


}
