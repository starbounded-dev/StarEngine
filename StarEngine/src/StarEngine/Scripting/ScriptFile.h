#pragma once

#include "StarEngine/Asset/Asset.h"

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
