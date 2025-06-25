#include "sepch.h"
#include "PipelineCompute.h"

#include "StarEngine/Renderer/RendererAPI.h"
#include "StarEngine/Renderer/ComputePipeline.h"

namespace StarEngine
{
	PipelineCompute::PipelineCompute(RefPtr<Shader> computeShader)
		: m_Shader(computeShader)
	{
		RefPtr<PipelineCompute> instance = this;
		Renderer::Submit([instance]() mutable
			{
				instance->RT_CreatePipeline();
			});
		Renderer::RegisterShaderDependency(computeShader, this);
	}

	void PipelineCompute::RT_CreatePipeline()
	{
		nvrhi::ComputePipelineDesc desc;
		desc.CS = m_Shader->GetHandle();
		//Todo binding layout;

		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();
		device->createComputePipeline(desc);
	}

	void PipelineCompute::Begin(RefPtr<RenderCommandBuffer> renderCommandBuffer /*= nullptr*/)
	{

	}

	void PipelineCompute::RT_Begin(RefPtr<RenderCommandBuffer> renderCommandBuffer /*= nullptr*/)
	{

	}

	void PipelineCompute::End()
	{

	}

	void PipelineCompute::BufferMemoryBarrier(RefPtr<RenderCommandBuffer> renderCommandBuffer, RefPtr<StorageBuffer> storageBuffer, ResourcesAccessFlags fromAccess, ResourcesAccessFlags toAccess)
	{

	}

	void PipelineCompute::BufferMemoryBarrier(RefPtr<RenderCommandBuffer> renderCommandBuffer, RefPtr<StorageBuffer> storageBuffer, PipelineStage fromStage, ResourcesAccessFlags fromAccess, PipelineStage toStage, ResourcesAccessFlags toAccess)
	{

	}

	void PipelineCompute::ImageMemoryBarrier(RefPtr<RenderCommandBuffer> renderCommandBuffer, RefPtr<Image2D> image, ResourcesAccessFlags fromAccess, ResourcesAccessFlags toAccess)
	{

	}

	void PipelineCompute::ImageMemoryBarrier(RefPtr<RenderCommandBuffer> renderCommandBuffer, RefPtr<Image2D> image, PipelineStage, fromStage, ResourcesAccessFlags fromAccess, PipelineStage toStage, ResourcesAccessFlags toAccess)
	{

	}

	void PipelineCompute::Execute(void* descriptorSets, uint32_t descriptorSetCount, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();

		nvrhi::ComputeState	computeState;
		computeState.pipeline = m_Handle;

		nvrhi::CommandListHandle commandList = device->createCommandList();
		commandList->open();

		commandList->setComputeState(computeState);
		commandList->dispatch(groupCountX, groupCountY, groupCountZ);

		commandList->close();
		device->executeCommandList(commandList);
	}


	void PipelineCompute::SetPushConstants(Buffer constants) const
	{

	}


	void PipelineCompute::CreatePipeline()
	{

	}

	RefPtr<Shader> PipelineCompute::GetShader() const
	{

	}

	RefPtr<PipelineCompute> PipelineCompute::Create(RefPtr<Shader> computeShader)
	{

	}
}
