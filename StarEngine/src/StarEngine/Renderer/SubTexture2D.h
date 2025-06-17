#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

namespace StarEngine
{
	class SubTexture2D : public Texture2D
	{
	public:
		SubTexture2D(const RefPtr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const RefPtr<Texture2D> GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		static RefPtr<SubTexture2D> CreateFromCoords(const RefPtr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1, 1 });
	private:
		RefPtr<Texture2D> m_Texture;

		glm::vec2 m_TexCoords[4];
	};
}

