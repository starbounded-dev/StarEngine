#pragma once

#include "Project.h"

namespace StarEngine {

	class ProjectSerializer : public RefCounted
	{
	public:
		ProjectSerializer(RefPtr<Project> project);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		RefPtr<Project> m_Project;
	};

}
