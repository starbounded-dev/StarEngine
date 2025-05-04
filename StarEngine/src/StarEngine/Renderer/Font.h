#pragma once

#include <filesystem>

#include "StarEngine/Core/Base.h"
#include "StarEngine/Renderer/Texture.h"

namespace StarEngine {

	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& font);
		~Font();

		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }
	private:
		MSDFData* m_Data;
		Ref<Texture2D> m_AtlasTexture;
	};


}
