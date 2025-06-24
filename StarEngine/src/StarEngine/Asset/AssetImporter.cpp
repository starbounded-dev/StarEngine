#include "sepch.h"
#include "AssetImporter.h"

#include "TextureImporter.h"
#include "SceneImporter.h"
#include "AudioImporter.h"


/*
#include "FontImporter.h"
#include "ObjModelImporter.h"*/

#include <map>

namespace StarEngine {

	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{ AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Texture2D, TextureImporter::ImportTexture2D },
		{ AssetType::Audio, AudioImporter::ImportAudio },
//		{ AssetType::ObjModel, ObjModelImporter::ImportObjModel },
		{ AssetType::ScriptFile, SceneImporter::ImportScript }
	};

	/**
	 * @brief Imports an asset using the appropriate importer based on asset metadata.
	 *
	 * Attempts to locate and invoke the registered import function for the asset type specified in the metadata.
	 * If no importer is available for the given asset type, logs an error and returns nullptr.
	 *
	 * @param handle The handle identifying the asset to import.
	 * @param metadata Metadata describing the asset, including its type.
	 * @return Ref<Asset> Reference-counted pointer to the imported asset, or nullptr if import fails due to missing importer.
	 */
	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		SE_PROFILE_FUNCTION_COLOR("AssetImporter::ImportAsset", 0xF2FA8A);

		{
			SE_PROFILE_SCOPE_COLOR("AssetImporter::ImportAsset Scope", 0x27628A);

			if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
			{
				SE_CORE_ERROR("No importer available for asset type: {}", (uint16_t)metadata.Type);
				return nullptr;
			}
		}

		auto& result = s_AssetImportFunctions.at(metadata.Type);//(metadata.Type)(handle, metadata);

		{
			SE_PROFILE_SCOPE_COLOR("AssetImporter::ImportAsset 2 Scope", 0xD1C48A);

			return result(handle, metadata);
		}

	}

}
