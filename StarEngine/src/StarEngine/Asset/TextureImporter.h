#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "StarEngine/Renderer/Texture.h"

namespace StarEngine {

	class TextureImporter
	{
	public:
		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
	};
}
