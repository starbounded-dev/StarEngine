#pragma once

#include "StarEngine/Core/Base.h"
#include "StarEngine/Core/Log.h"

#include "StarEngine/Scene/Scene.h"
#include "StarEngine/Scene/Entity.h"

namespace StarEngine {
    class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}