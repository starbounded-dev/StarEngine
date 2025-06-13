#pragma once

#include "StarEngine/Renderer/VertexBuffer.h"
#include "StarEngine/Renderer/IndexBuffer.h"

#include "StarEngine/Core/Core.h"

#include <memory>

namespace StarEngine {

	class VertexArray : public RefCounted 
	{

		public:
			virtual ~VertexArray() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual void AddVertexBuffer(const RefPtr<VertexBuffer>& vertexBuffer) = 0;
			virtual void SetIndexBuffer(const RefPtr<IndexBuffer>& indexBuffer) = 0;

			virtual const std::vector<RefPtr<VertexBuffer>>& GetVertexBuffers() const = 0;
			virtual const RefPtr<IndexBuffer>& GetIndexBuffer() const = 0;

			static RefPtr<VertexArray> Create();
	};
}
