#pragma once

#include "StarEngine/Renderer/Texture.h"

#include <filesystem>

namespace StarEngine
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;

		std::filesystem::path m_BaseDirectory;
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};

}
