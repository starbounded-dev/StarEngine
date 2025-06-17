#include "sepch.h"
#include "SceneImporter.h"

#include "StarEngine/Project/Project.h"
#include "StarEngine/Scene/SceneSerializer.h"
#include "StarEngine/Scripting/ScriptEngine.h"

#include <stb_image.h>

namespace StarEngine {

	RefPtr<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		//SE_PROFILE_FUNCTION();

		return LoadScene(Project::GetActiveAssetDirectory() / metadata.FilePath);
	}

	RefPtr<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		//SE_PROFILE_FUNCTION();

		RefPtr<Scene> scene = RefPtr<Scene>::Create();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);

		return scene;
	}

	void SceneImporter::SaveScene(RefPtr<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}
}
