#pragma once

#include "StarEngine/Renderer/OrthographicCamera.h"
#include "StarEngine/Renderer/Texture.h"

#include "StarEngine/Renderer/SubTexture2D.h"

#include "StarEngine/Renderer/Camera.h"
#include "StarEngine/Renderer/EditorCamera.h"
#include "StarEngine/Renderer/Font.h"

#include "StarEngine/Scene/Components.h"

namespace StarEngine {
	class Renderer2D
	{
	public:

		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);

		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera); // TODO: Remove
		static void EndScene();
		static void Flush();

		// Native
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		// Texture2D
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const RefPtr<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const RefPtr<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		// SubTexture2D
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const RefPtr<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const RefPtr<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const RefPtr<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);

		// Rotated Native
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);

		// Rotated Texture2D
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const RefPtr<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const RefPtr<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Rotated SubTexture2D
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const RefPtr<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const RefPtr<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Circle
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		// Line
		static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);

		// Rect
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		// Sprite
		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);

		// Text
		struct TextParams
		{
			glm::vec4 Color{ 1.0f };
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};
		static void DrawString(const std::string& string, RefPtr<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID = -1);
		static void DrawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID = -1);

		static float GetLineWidth();
		static void SetLineWidth(float width);

		// Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};

		static void ResetStats();
		static Statistics GetStats();
	private:
		static void StartBatch();
		static void NextBatch();
	};
}
