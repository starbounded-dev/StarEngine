#pragma once

#include "StarEngine/Core/Base.h"

#include "AssetMetadata.h"

namespace StarEngine
{
	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};


}
