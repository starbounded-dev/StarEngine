#pragma once

#include "StarEngine/Core/Ref.h"
#include "StarEngine/Renderer/Shader.h"
#include "StarEngine/Renderer/StorageBuffer.h"
#include "StarEngine/Renderer/RenderCommandBuffer.h"

#include <nvrhi/nvrhi.h>

namespace StarEngine
{
	class PipelineCompute : public RefCounted
	{
		static RefPtr<PipelineCompute> Create(RefPtr<Shader> computeShader);

		void Begin(RefPtr<RenderCommandBuffer> renderCommandBuffer = nullptr);
		void RT_Begin(RefPtr<RenderCommandBuffer> renderCommandBuffer = nullptr);
		void End();

		void BufferMemoryBarrier(RefPtr<RenderCommandBuffer> renderCommandBuffer, RefPtr<StorageBuffer> storageBuffer, ResourcesAccessFlags fromAccess, ResourcesAccessFlags toAccess);
		void BufferMemoryBarrier(RefPtr<RenderCommandBuffer> renderCommandBuffer, RefPtr<StorageBuffer> storageBuffer, PipelineStage fromStage, ResourcesAccessFlags fromAccess, PipelineStage toStage, ResourcesAccessFlags toAccess);

		void ImageMemoryBarrier(RefPtr<RenderCommandBuffer> renderCommandBuffer, RefPtr<Image2D> image, ResourcesAccessFlags fromAccess, ResourcesAccessFlags toAccess);
		void ImageMemoryBarrier(RefPtr<RenderCommandBuffer> renderCommandBuffer, RefPtr<Image2D> image, PipelineStage, fromStage, ResourcesAccessFlags fromAccess, PipelineStage toStage, ResourcesAccessFlags toAccess);

		void Execute(void* descriptorSets, uint32_t descriptorSetCount, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

		void SetPushConstants(Buffer constants) const;
		void CreatePipeline();

		RefPtr<Shader> GetShader() const;

	public:
		PipelineCompute(RefPtr<Shader> computeShader);
	private:
		void RT_CreatePipeline();
	private:
		RefPtr<Shader> m_Shader;
		nvrhi::ComputePipelineHandle m_Pipeline = nullptr;

		bool m_UsingGraphicsQueue = false;
	};
}
