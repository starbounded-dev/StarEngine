#include "sepch.h"
#include "PipelineCompute.h"

#include "StarEngine/Core/Application.h"
#include "StarEngine/Renderer/RendererAPI.h"
#include "StarEngine/Renderer/ComputePipeline.h"

namespace StarEngine
{
	PipelineCompute::PipelineCompute(Ref<Shader> computeShader)
		: m_Shader(computeShader)
	{
		Ref<PipelineCompute> instance = this;
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

	void PipelineCompute::Begin(Ref<RenderCommandBuffer> renderCommandBuffer /*= nullptr*/)
	{

	}

	void PipelineCompute::RT_Begin(Ref<RenderCommandBuffer> renderCommandBuffer /*= nullptr*/)
	{

	}

	void PipelineCompute::End()
	{

	}

	void PipelineCompute::BufferMemoryBarrier(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<StorageBuffer> storageBuffer, ResourcesAccessFlags fromAccess, ResourcesAccessFlags toAccess)
	{

	}

	void PipelineCompute::BufferMemoryBarrier(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<StorageBuffer> storageBuffer, PipelineStage fromStage, ResourcesAccessFlags fromAccess, PipelineStage toStage, ResourcesAccessFlags toAccess)
	{

	}

	void PipelineCompute::ImageMemoryBarrier(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> image, ResourcesAccessFlags fromAccess, ResourcesAccessFlags toAccess)
	{

	}

	void PipelineCompute::ImageMemoryBarrier(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> image, PipelineStage fromStage, ResourcesAccessFlags fromAccess, PipelineStage toStage, ResourcesAccessFlags toAccess)
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

	Ref<Shader> PipelineCompute::GetShader() const
	{

	}

	Ref<PipelineCompute> PipelineCompute::Create(Ref<Shader> computeShader)
	{

	}
}
