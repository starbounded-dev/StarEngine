#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "StarEngine/Renderer/Texture.h"

namespace StarEngine {

	class TextureImporter : RefCounted
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static RefPtr<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);

		// Reads file directly from filesystem
		// (i.e. path has to be relative / absolute to working directory)
		static RefPtr<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};
}
