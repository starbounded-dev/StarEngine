#pragma once

#define YAML_CPP_STATIC_DEFINE

#include "StarEngine/Scene/Scene.h"

namespace StarEngine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const RefPtr<Scene>& scene);

		void Serialize(const std::filesystem::path& filepath);
		void SerializeRuntime(const std::filesystem::path& filepath);

		bool Deserialize(const std::filesystem::path& filepath);
		bool DeserializeRuntime(const std::filesystem::path& filepath);
	private:
		RefPtr<Scene> m_Scene;
	};
}
