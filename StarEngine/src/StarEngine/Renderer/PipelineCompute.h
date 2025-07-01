#pragma once

#include "StarEngine/Core/Ref.h"
#include "StarEngine/Renderer/Shader.h"
#include "StarEngine/Renderer/StorageBuffer.h"
#include "StarEngine/Renderer/RenderCommandBuffer.h"
#include "StarEngine/Renderer/Image.h"
#include "StarEngine/Renderer/Pipeline.h"

#include <nvrhi/nvrhi.h>

namespace StarEngine
{
	class PipelineCompute : public RefCounted
	{
		static Ref<PipelineCompute> Create(Ref<Shader> computeShader) { return Ref<PipelineCompute>::Create(computeShader); }

		void Begin(Ref<RenderCommandBuffer> renderCommandBuffer = nullptr);
		void RT_Begin(Ref<RenderCommandBuffer> renderCommandBuffer = nullptr);
		void End();

		void BufferMemoryBarrier(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<StorageBuffer> storageBuffer, ResourcesAccessFlags fromAccess, ResourcesAccessFlags toAccess);
		void BufferMemoryBarrier(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<StorageBuffer> storageBuffer, PipelineStage fromStage, ResourcesAccessFlags fromAccess, PipelineStage toStage, ResourcesAccessFlags toAccess);

		void ImageMemoryBarrier(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> image, ResourcesAccessFlags fromAccess, ResourcesAccessFlags toAccess);
		void ImageMemoryBarrier(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> image, PipelineStage fromStage, ResourcesAccessFlags fromAccess, PipelineStage toStage, ResourcesAccessFlags toAccess);

		void Execute(void* descriptorSets, uint32_t descriptorSetCount, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

		void SetPushConstants(Buffer constants) const;
		void CreatePipeline();

		Ref<Shader> GetShader() const { return m_Shader; }

	public:
		PipelineCompute(Ref<Shader> computeShader);
	private:
		void RT_CreatePipeline();
	private:
		Ref<Shader> m_Shader;
		nvrhi::ComputePipelineHandle m_Pipeline = nullptr;

		bool m_UsingGraphicsQueue = false;
	};
}
