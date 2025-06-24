#include "sepch.h"
#include "SceneImporter.h"

#include "StarEngine/Project/Project.h"
#include "StarEngine/Scene/SceneSerializer.h"
#include "StarEngine/Scripting/ScriptEngine.h"

#include <stb_image.h>

namespace StarEngine {

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		//SE_PROFILE_FUNCTION();

		return LoadScene(Project::GetActiveAssetDirectory() / metadata.FilePath);
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		//SE_PROFILE_FUNCTION();

		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);

		return scene;
	}

	/**
	 * @brief Serializes and saves a scene to a file within the active asset directory.
	 *
	 * @param scene Reference-counted pointer to the scene to be saved.
	 * @param path Relative file path where the scene will be saved.
	 */
	void SceneImporter::SaveScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}

	/**
	 * @brief Imports a script asset using its handle and metadata.
	 *
	 * Constructs the full file path from the active asset directory and the provided metadata, then loads the script from this path.
	 *
	 * @return Reference-counted pointer to the loaded script.
	 */
	Ref<Script> SceneImporter::ImportScript(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadScript(Project::GetActiveAssetDirectory() / metadata.FilePath);
	}

	/**
	 * @brief Creates and returns a new, empty Script instance.
	 *
	 * @return Ref<Script> A reference-counted pointer to a newly constructed Script.
	 */
	Ref<Script> SceneImporter::LoadScript(const std::filesystem::path& path)
	{
		Ref<Script> result = std::make_shared<Script>();

		return result;
	}
}
