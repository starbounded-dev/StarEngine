#pragma once

#include "StarEngine/Core/Base.h"

#include "AssetMetadata.h"

/**
 * Imports an asset using the specified handle and metadata.
 * @param handle The handle identifying the asset to import.
 * @param metadata Metadata describing the asset to be imported.
 * @return A reference-counted pointer to the imported asset.
 */
namespace StarEngine
{
	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};


}
