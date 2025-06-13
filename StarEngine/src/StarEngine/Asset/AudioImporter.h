#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "StarEngine/Audio/AudioSource.h"

namespace StarEngine {

	class AudioImporter : public RefCounted
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static RefPtr<AudioSource> ImportAudio(AssetHandle handle, const AssetMetadata& metadata);

		// Load from filepath
		static RefPtr<AudioSource> LoadAudio(const std::filesystem::path& path);
	};

}
