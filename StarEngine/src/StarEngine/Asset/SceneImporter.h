#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "StarEngine/Scene/Scene.h"
#include "StarEngine/Scripting/ScriptFile.h"

/**
 * Imports a scene asset using the provided asset handle and metadata.
 * @param handle The handle identifying the asset to import.
 * @param metadata Metadata describing the asset; the file path is relative to the project asset directory.
 * @return Reference to the imported Scene object.
 */

/**
 * Loads a scene from the specified file system path.
 * @param path Path to the scene file.
 * @return Reference to the loaded Scene object.
 */

/**
 * Saves the given scene to the specified file system path.
 * @param scene Reference to the Scene object to save.
 * @param path Path where the scene will be saved.
 */

/**
 * Imports a script asset using the provided asset handle and metadata.
 * @param handle The handle identifying the script asset to import.
 * @param metadata Metadata describing the script asset.
 * @return Reference to the imported Script object.
 */

/**
 * Loads a script from the specified file system path.
 * @param path Path to the script file.
 * @return Reference to the loaded Script object.
 */
namespace StarEngine {

	class SceneImporter
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);

		// Load from filepath
		static Ref<Scene> LoadScene(const std::filesystem::path& path);

		static void SaveScene(Ref<Scene> scene, const std::filesystem::path& path);

		static Ref<Script> ImportScript(AssetHandle handle, const AssetMetadata& metadata);

		static Ref<Script> LoadScript(const std::filesystem::path& path);
	};

}
