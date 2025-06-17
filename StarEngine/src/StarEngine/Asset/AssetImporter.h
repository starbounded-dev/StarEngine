#pragma once

#include "AssetMetadata.h"

namespace StarEngine
{
	class AssetImporter
	{
	public:
		static RefPtr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};


}
