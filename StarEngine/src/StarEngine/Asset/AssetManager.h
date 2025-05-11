#pragma once

#include "AssetManagerBase.h"

#include "StarEngine/Project/Project.h"

namespace StarEngine
{
	class AssetManager
	{
	public:
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			Ref<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}
	};
}
