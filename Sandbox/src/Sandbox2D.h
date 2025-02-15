#pragma once

#include "StarEngine.h"
#include "StarEngine/Renderer/OrthographicCameraController.h"

#include "ParticleSystem.h"

class Sandbox2D : public StarEngine::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(StarEngine::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(StarEngine::Event& e) override;
private:
	StarEngine::OrthographicCameraController m_CameraController;

	bool m_VSync = true;

	// Temp
	StarEngine::Ref<StarEngine::VertexArray> m_SquareVA;
	StarEngine::Ref<StarEngine::Shader> m_FlatColorShader;

	StarEngine::Ref<StarEngine::Texture2D> m_CheckerboardTexture;
	StarEngine::Ref<StarEngine::Texture2D> m_SpriteSheet;

	StarEngine::Ref<StarEngine::SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;
};
