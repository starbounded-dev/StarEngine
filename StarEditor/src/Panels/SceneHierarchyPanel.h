#pragma once

#include "StarEngine/Core/Ref.h"
#include "StarEngine/Scene/Scene.h"
#include "StarEngine/Scene/Entity.h"

namespace StarEngine {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const RefPtr<Scene>& scene);

		void SetContext(const RefPtr<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		RefPtr<Scene> m_Context;
		Entity m_SelectionContext;
	};

}
