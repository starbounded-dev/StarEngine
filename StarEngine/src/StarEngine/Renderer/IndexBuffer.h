#pragma once

#include "StarEngine/Core/Ref.h"
#include "StarEngine/Core/Buffer.h"

#include <nvrhi/nvrhi.h>

namespace StarEngine {

	class IndexBuffer : public RefCounted
	{
	public:
		static RefPtr<IndexBuffer> Create(const Buffer& buffer) { return RefPtr<IndexBuffer>::Create(buffer); }
		static RefPtr<IndexBuffer> Create(uint64_t size) { return RefPtr<IndexBuffer>::Create(size); }

		void SetData(void* buffer, uint64_t size, uint64_t offset = 0);
		
		uint64_t GetSize() const { return m_Size; }
		uint32_t GetCount() const { return (uint32_t)(m_Size / 4); }

		nvrhi::BufferHandle GetHandle() const { return m_Handle; }

	public:
		IndexBuffer(const Buffer& buffer);
		IndexBuffer(uint64_t size);

		virtual ~IndexBuffer() = default;

	private:
		nvrhi::BufferHandle m_Handle;

		uint64_t m_Size = 0;
	};
}
