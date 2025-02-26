#pragma once

#include "StarEngine.h"
#include "Panels/SceneHierarchyPanel.h"
#include "StarEngine/Scene/Entity.h"

#include "entt.hpp"

namespace StarEngine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		OrthographicCameraController m_CameraController;

		bool m_VSync = true;
		bool m_ViewportFocused = false, m_ViewportHovered = false;

		// Temp
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		bool m_PrimaryCamera = true;

		Ref<Texture2D> m_CheckerboardTexture;

		glm::vec2 m_ViewportSize = {0.0f, 0.0f};

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}

