#pragma once

#include "StarEngine/Asset/Asset.h"

/**
 * Represents a script asset within the engine.
 *
 * Inherits from Asset and identifies itself as an AssetType::ScriptFile.
 */
namespace StarEngine {

	class Script : public Asset
	{
	public:
		Script() = default;
		~Script() = default;

		static AssetType GetStaticType() { return AssetType::ScriptFile; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	};

}
