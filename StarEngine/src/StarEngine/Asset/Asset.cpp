#include "sepch.h"
#include "Asset.h"

namespace StarEngine {

	/**
	 * @brief Converts an AssetType enum value to its string representation.
	 *
	 * @param type The AssetType value to convert.
	 * @return std::string_view The corresponding string name, or "<Invalid>" if the type is unrecognized.
	 */
	std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::None:        return "None";
		case AssetType::Scene:       return "Scene";
		case AssetType::Texture2D:   return "Texture2D";
		case AssetType::Audio:       return "Audio";
		//case AssetType::ObjModel:    return "ObjModel";
		case AssetType::ScriptFile:  return "ScriptFile";
		}

		return "<Invalid>";
	}

	/**
	 * @brief Converts a string representation of an asset type to its corresponding AssetType enum value.
	 *
	 * If the input string does not match any known asset type, returns AssetType::None.
	 *
	 * @param assetType The string representation of the asset type.
	 * @return AssetType The corresponding enum value, or AssetType::None if unrecognized.
	 */
	AssetType AssetTypeFromString(std::string_view assetType)
	{
		if (assetType == "None")      return AssetType::None;
		if (assetType == "Scene")     return AssetType::Scene;
		if (assetType == "Texture2D") return AssetType::Texture2D;
		if (assetType == "Audio")     return AssetType::Audio;
		//if (assetType == "ObjModel")  return AssetType::ObjModel;
		if (assetType == "ScriptFile")  return AssetType::ScriptFile;

		return AssetType::None;
	}

}
