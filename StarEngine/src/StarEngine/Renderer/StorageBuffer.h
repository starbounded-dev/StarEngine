#pragma once

#include "StarEngine/Core/Ref.h"

#include <string>

namespace StarEngine {

	struct StorageBufferSpecification
	{
		bool GPUOnly = true;
		std::string DebugName;
	};

	class StorageBuffer : public RefCounted
	{
	public:
		virtual ~StorageBuffer() = default;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void RT_SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Resize(uint32_t newSize) = 0;

		static Ref<StorageBuffer> Create(uint32_t size, const StorageBufferSpecification& specification);
	};

}
