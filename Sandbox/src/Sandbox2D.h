#pragma once
#include "StarStudio.h"
#include "StarStudio/Renderer/OrthographicCameraController.h"

class Sandbox2D : public StarStudio::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(StarStudio::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(StarStudio::Event& e) override;
private:
	StarStudio::OrthographicCameraController m_CameraController;

	// Temp
	StarStudio::Ref<StarStudio::VertexArray> m_SquareVA;
	StarStudio::Ref<StarStudio::Shader> m_FlatColorShader;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};
