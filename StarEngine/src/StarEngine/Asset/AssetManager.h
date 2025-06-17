#pragma once

#include "AssetManagerBase.h"

#include "StarEngine/Project/Project.h"

namespace StarEngine
{
	class AssetManager : public RefCounted
	{
	public:
		template<typename T>
		static RefPtr<T> GetAsset(AssetHandle handle)
		{
			//SE_PROFILE_FUNCTION_COLOR("AssetManager::GetAsset", 0x8CCBFF);

			RefPtr<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static bool IsAssetHandleValid(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->IsAssetHandleValid(handle);
		}

		static bool IsAssetLoaded(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->IsAssetLoaded(handle);
		}

		static AssetType GetAssetType(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->GetAssetType(handle);
		}
	};
}
