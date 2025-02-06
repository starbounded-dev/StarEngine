#pragma once

#include "StarStudio.h"
#include "StarStudio/Renderer/OrthographicCameraController.h"

class ExampleLayer : public StarStudio::Layer
{

public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(StarStudio::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(StarStudio::Event& e) override;
private:
	StarStudio::ShaderLibrary m_ShaderLibrary;
	StarStudio::Ref<StarStudio::Shader> m_Shader;
	StarStudio::Ref<StarStudio::VertexArray> m_VertexArray;

	StarStudio::Ref<StarStudio::Shader> m_FlatColorShader;
	StarStudio::Ref<StarStudio::VertexArray> m_SquareVA;

	StarStudio::Ref<StarStudio::Texture2D> m_Texture, m_starLogoTexture;

	StarStudio::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};
