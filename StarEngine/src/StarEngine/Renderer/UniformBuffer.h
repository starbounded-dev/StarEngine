#pragma once

#include "StarEngine/Core/Ref.h"

namespace StarEngine {

	class UniformBuffer : public RefCounted
	{
	public:
		virtual ~UniformBuffer() {}
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static RefPtr<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};

}
