#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "StarEngine/Scene/Scene.h"

namespace StarEngine {

	class SceneImporter : RefCounted
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static RefPtr<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);

		// Load from filepath
		static RefPtr<Scene> LoadScene(const std::filesystem::path& path);

		static void SaveScene(RefPtr<Scene> scene, const std::filesystem::path& path);
	};

}
