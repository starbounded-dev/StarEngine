#pragma once

#include "StarEngine/Core/Ref.h"
#include "StarEngine/Core/Buffer.h"

#include <nvrhi/nvrhi.h>

namespace StarEngine {

	class IndexBuffer : public RefCounted
	{
	public:
		// Static Create method for creating an IndexBuffer with a Buffer
		static RefPtr<IndexBuffer> Create(const Buffer& buffer)
		{
			return std::make_shared<IndexBuffer>(buffer);
		}

		// Static Create method for creating an IndexBuffer with a size
		static RefPtr<IndexBuffer> Create(uint64_t size)
		{
			return std::make_shared<IndexBuffer>(size);
		}

		virtual ~IndexBuffer() = default;

		void SetData(void* buffer, uint64_t size, uint64_t offset = 0);

		uint64_t GetSize() const { return m_Size; }
		uint64_t GetCount() const { return m_Size / 4; }

		nvrhi::BufferHandle GetHandle() const { return m_Handle; }

	public:
		// Private constructors to enforce the use of the static Create methods
		IndexBuffer(const Buffer& buffer) : m_Handle(nullptr), m_Size(buffer.Size) {}
		IndexBuffer(uint64_t size) : m_Handle(nullptr), m_Size(size) {}

	private:
		nvrhi::BufferHandle m_Handle;
		uint64_t m_Size = 0;
	};
}
