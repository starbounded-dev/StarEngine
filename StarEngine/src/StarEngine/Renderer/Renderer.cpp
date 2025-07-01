#include "sepch.h"
#include "Renderer.h"

#include "Shader.h"

#include "RendererAPI.h"

#include "StarEngine/Core/Timer.h"
#include "StarEngine/Debug/Profiler.h"
#include "StarEngine/Project/Project.h"

#include <filesystem>
#include <format>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "StarEngine/Audio/AudioEngine.h"

namespace std {
	template<>
	struct hash<StarEngine::WeakRef<StarEngine::Shader>>
	{
		size_t operator()(const StarEngine::WeakRef<StarEngine::Shader>& shader) const noexcept
		{
			return shader->GetHash();
		}
	};
}

namespace StarEngine {

	static GlobalShaderInfo s_GlobalShaderInfo;

	struct RendererData
	{
		RendererCapabilities RenderCaps;

		Ref<Texture2D> BRDFLut;

		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;
		Shader::ShaderMaterialDescriptorSet QuadDescriptorSet;

		std::unordered_map<SceneRenderer*, std::vector<Shader::ShaderMaterialDescriptorSet>> RendererDescriptorSet;
		DescriptorSet ActiveRendererDescriptorSet = nullptr;
		std::vector<DescriptorPool> DescriptorPools;
		DescriptorPool MaterialDescriptorPool;
		std::vector<uint32_t> DescriptorPoolAllocationCount;

		std::unordered_map<UniformBufferSet*, std::unordered_map<uint64_t, std::vector<std::vector<WriteDescriptorSet>>>> UniformBufferWriteDescriptorCache;
		std::unordered_map<StorageBufferSet*, std::unordered_map<uint64_t, std::vector<std::vector<WriteDescriptorSet>>>> StorageBufferWriteDescriptorCache;

		nvrhi::SamplerHandle SamplerClamp = nullptr;
		nvrhi::SamplerHandle SamplerPoint = nullptr;

		int32_t SelectedDrawCall = -1;
		int32_t DrawCallCount = 0;
	};

	static RendererData* s_RendererData = nullptr;
	
	typedef void(*Renderer::RenderCommandFn(*RenderCommandFn) (void*)
	{

	}

	void Renderer::Init()
	{

	}

	void Renderer::Shutdown()
	{
		{
			std::scoped_lock lock(s_ShaderDependenciesMutex);
			s_ShaderDependencies.clear();
		}

		VkDevice device = Context::GetCurrentDevice()->GetDevice();
		vkDeviceWaitIdle(device);

		if (s_RendererData->SamplerPoint)
		{
			Vulkan::DestroySampler(s_RendererData->SamplerPoint);
			s_RendererData->SamplerPoint = nullptr;
		}

		if (s_RendererData->SamplerClamp)
		{
			Vulkan::DestroySampler(s_RendererData->SamplerClamp);
			s_RendererData->SamplerClamp = nullptr;
		}

#if SE_HAS_SHADER_COMPILED
		VulkanShaderCompiler::ClearUniformBuffer();
#endif
		delete s_RendererData;

		delete s_Data;

		for (uint32_t i = 0; i < s_Config.FramesInFlight; i++)
		{
			auto& queue = Renderer::GetRenderResourceReleaseQueue(i);
			queue.Execute();
		}

		delete s_CommandQueue[0];
		delete s_CommandQueue[1];
	}

	RendererCapabilities& Renderer::GetCapabilities()
	{

	}

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{

	}

	void Renderer::WaitAndRender(RenderThread* renderThread)
	{

	}

	void Renderer::SwapQueues()
	{

	}

	void Renderer::RenderThreadFunc(RenderThread* renderThread)
	{

	}

	uint32_t Renderer::GetRenderQueueIndex()
	{

	}

	uint32_t Renderer::GetRenderQueueSubmissionIndex()
	{

	}

	void Renderer::BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass, bool explicitClear)
	{

	}

	void Renderer::EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer)
	{

	}

	void Renderer::BeginGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer, const std::string& label, const glm::vec4& markerColor)
	{

	}

	void Renderer::InsertGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer, const std::string& label, const glm::vec4& markerColor)
	{

	}

	void Renderer::EndGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer)
	{

	}

	void Renderer::RT_BeginGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer, const std::string& label, const glm::vec4& markerColor)
	{

	}

	void Renderer::RT_InsertGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer, const std::string& label, const glm::vec4& markerColor)
	{

	}

	void Renderer::RT_EndGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer)
	{

	}

	void Renderer::BeginFrame()
	{

	}

	void Renderer::EndFrame()
	{

	}

	void Renderer::RenderStaticMesh(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<StaticMesh> mesh, Ref<MeshSource> meshSource, uint32_t submeshIndex, Ref<MaterialTable> materialTable, Ref<VertexBuffer> transformBuffer, uint32_t transformOffset, uint32_t instanceCount)
	{

	}

	void Renderer::RenderStaticMeshWithMaterial(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<StaticMesh> mesh, Ref<MeshSource> meshSource, uint32_t submeshIndex, Ref<VertexBuffer> transformBuffer, uint32_t transformOffset, uint32_t instanceCount, Ref<Material> material, Buffer additionalUniforms)
	{

	}

	void Renderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount)
	{

	}

	void Renderer::RenderQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, const glm::mat4& transform)
	{

	}

	void Renderer::SubmitFullscreenQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material)
	{

	}

	void Renderer::SubmitFullscreenQuadWithOverrides(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Buffer vertexShaderOverrides, Buffer fragmentShaderOverrides)
	{

	}

	void Renderer::ClearImage(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> image, const ImageClearValue& clearValue, ImageSubresourceRange subresourceRange)
	{

	}

	void Renderer::CopyImage(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> sourceImage, Ref<Image2D> destinationImage)
	{

	}

	void Renderer::BlitImage(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> sourceImage, Ref<Image2D> destinationImage)
	{

	}

	Ref<Texture2D> Renderer::GetWhiteTexture()
	{

	}

	Ref<Texture2D> Renderer::GetBlackTexture()
	{

	}

	Ref<Texture2D> Renderer::GetHilbertLut()
	{

	}

	Ref<Texture2D> Renderer::GetBRDFLutTexture()
	{

	}

	Ref<TextureCube> Renderer::GetBlackCubeTexture()
	{

	}

	void Renderer::RegisterShaderDependency(Ref<Shader> shader, Ref<Pipeline> pipeline)
	{

	}

	void Renderer::RegisterShaderDependency(Ref<Shader> shader, Ref<Material> material)
	{

	}

	void Renderer::OnShaderReloaded(size_t hash)
	{

	}

	uint32_t Renderer::GetCurrentFrameIndex()
	{

	}

	uint32_t Renderer::RT_GetCurrentFrameIndex()
	{

	}

	RendererConfig& Renderer::GetConfig()
	{

	}

	void Renderer::SetConfig(const RendererConfig& config)
	{

	}

	RenderCommandQueue& Renderer::GetRenderResourceReleaseQueue(uint32_t index)
	{

	}

	const std::unordered_map<std::string, std::string>& Renderer::GetGlobalShaderMacros()
	{

	}

	void Renderer::AcknowledgeParsedGlobalMacros(const std::unordered_set<std::string>& macros, Ref<Shader> shader)
	{

	}

	void Renderer::SetMacroInShader(Ref<Shader> shader, const std::string& name, const std::string& value)
	{

	}

	void Renderer::SetGlobalMacroInShaders(const std::string& name, const std::string& value)
	{

	}

	bool Renderer::UpdateDirtyShaders()
	{

	}

	GPUMemoryStats Renderer::GetGPUMemoryStats()
	{

	}

	nvrhi::SamplerHandle Renderer::GetClampSampler()
	{

	}

	nvrhi::SamplerHandle Renderer::GetPointSampler()
	{

	}

}
