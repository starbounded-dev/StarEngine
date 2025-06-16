#pragma once

#include "UniformBuffer.h"

namespace StarEngine
{
	class UniformBufferSet : public RefCounted
	{
		virtual ~UniformBufferSet() {}

		virtual RefPtr<UniformBuffer> Get() = 0;
		virtual RefPtr<UniformBuffer> RT_Get() = 0;
		virtual RefPtr<UniformBuffer> Get(uint32_t frame) = 0;

		virtual void Set(RefPtr<UniformBuffer> uniformBuffer, uint32_t frame) = 0;

		static RefPtr<UniformBuffer> Create(uint32_t size, uint32_t frameInFlight = 0);
	};
}
