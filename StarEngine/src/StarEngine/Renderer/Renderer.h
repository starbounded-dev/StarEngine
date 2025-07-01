#pragma once

#include "RendererContext.h"
#include "RenderCommandQueue.h"
#include "RenderPass.h"
#include "ComputePass.h"
#include "RenderCommandBuffer.h"
#include "PipelineCompute.h"
#include "UniformBufferSet.h"
#include "StorageBufferSet.h"

#include "StarEngine/Core/Application.h"
#include "StarEngine/Core/RenderThread.h"

#include "RendererCapabilities.h"
#include "RendererConfig.h"

#include "GPUStats.h"

#include <nvrhi/nvrhi.h>

#include "StarEngine/Scene/Scene.h"

namespace StarEngine {

	class ShaderLibrary;

	class Renderer
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		static Ref<RendererContext> GetContext()
		{
			return Application::Get().GetWindow().GetRenderContext();
		}

		static void Init();
		static void Shutdown();

		static RendererCapabilities& GetCapabilities();

		static Ref<ShaderLibrary> GetShaderLibrary();

		template<typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr)
				{
					auto pFunc = (FuncT*)ptr;
					(*pFunc)();

					// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
					// however some items like uniforms which contain std::strings still exist for now
					// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
					pFunc->~FuncT();
				};

			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		};

		template<typename FuncT>
		static void SubmitResourceFree(FuncT&& func)
		{
			auto renderCmd = [](void* ptr) {
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();

				// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
				// however some items like uniforms which contain std::strings still exist for now
				// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
				pFunc->~FuncT();
				};

			if (RenderThread::IsCurrentThreadRT())
			{
				const uint32_t index = Renderer::RT_GetCurrentFrameIndex();
				auto storageBuffer = GetRenderResourceReleaseQueue(index).Allocate(renderCmd, sizeof(func));
				new (storageBuffer) FuncT(std::forward<FuncT>((FuncT&&)func));
			}
			else
			{
				const uint32_t index = Renderer::GetCurrentFrameIndex();
				Submit([renderCmd, func, index]()
					{
						auto storageBuffer = GetRenderResourceReleaseQueue(index).Allocate(renderCmd, sizeof(func));
						new (storageBuffer) FuncT(std::forward<FuncT>((FuncT&&)func));
					});
			}
		}

		static void WaitAndRender(RenderThread* renderThread);
		static void SwapQueues();

		static void RenderThreadFunc(RenderThread* renderThread);
		static uint32_t GetRenderQueueIndex();
		static uint32_t GetRenderQueueSubmissionIndex();

		// ~Actual~ Renderer here... TODO: remove confusion later

		// Render Pass API
		static void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass, bool explicitClear = false);
		static void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer);

		static void BeginGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer, const std::string& label, const glm::vec4& markerColor = {});
		static void InsertGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer, const std::string& label, const glm::vec4& markerColor = {});
		static void EndGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer);

		static void RT_BeginGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer, const std::string& label, const glm::vec4& markerColor = {});
		static void RT_InsertGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer, const std::string& label, const glm::vec4& markerColor = {});
		static void RT_EndGPUPerfMarker(Ref<RenderCommandBuffer> renderCommandBuffer);

		static void BeginFrame();
		static void EndFrame();

		static void RenderStaticMesh(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<StaticMesh> mesh, Ref<MeshSource> meshSource, uint32_t submeshIndex, Ref<MaterialTable> materialTable, Ref<VertexBuffer> transformBuffer, uint32_t transformOffset, uint32_t instanceCount);
		static void RenderStaticMeshWithMaterial(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<StaticMesh> mesh, Ref<MeshSource> meshSource, uint32_t submeshIndex, Ref<VertexBuffer> transformBuffer, uint32_t transformOffset, uint32_t instanceCount, Ref<Material> material, Buffer additionalUniforms = Buffer());
		static void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0);
		static void RenderQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, const glm::mat4& transform);
		static void SubmitFullscreenQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material);
		static void SubmitFullscreenQuadWithOverrides(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Buffer vertexShaderOverrides, Buffer fragmentShaderOverrides);
		static void ClearImage(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> image, const ImageClearValue& clearValue, ImageSubresourceRange subresourceRange = ImageSubresourceRange());
		static void CopyImage(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> sourceImage, Ref<Image2D> destinationImage);
		static void BlitImage(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Image2D> sourceImage, Ref<Image2D> destinationImage);

		static Ref<Texture2D> GetWhiteTexture();
		static Ref<Texture2D> GetBlackTexture();
		static Ref<Texture2D> GetHilbertLut();
		static Ref<Texture2D> GetBRDFLutTexture();
		static Ref<TextureCube> GetBlackCubeTexture();

		static void RegisterShaderDependency(Ref<Shader> shader, Ref<Pipeline> pipeline);
		static void RegisterShaderDependency(Ref<Shader> shader, Ref<Material> material);
		static void OnShaderReloaded(size_t hash);

		static uint32_t GetCurrentFrameIndex();
		static uint32_t RT_GetCurrentFrameIndex();

		static RendererConfig& GetConfig();
		static void SetConfig(const RendererConfig& config);

		static RenderCommandQueue& GetRenderResourceReleaseQueue(uint32_t index);

		// Add known macro from shader.
		static const std::unordered_map<std::string, std::string>& GetGlobalShaderMacros();
		static void AcknowledgeParsedGlobalMacros(const std::unordered_set<std::string>& macros, Ref<Shader> shader);
		static void SetMacroInShader(Ref<Shader> shader, const std::string& name, const std::string& value = "");
		static void SetGlobalMacroInShaders(const std::string& name, const std::string& value = "");
		// Returns true if any shader is actually updated.
		static bool UpdateDirtyShaders();

		static GPUMemoryStats GetGPUMemoryStats();

		static nvrhi::SamplerHandle GetClampSampler();
		static nvrhi::SamplerHandle GetPointSampler();
	private:
		static RendererCommandQueue& GetRenderCommandQueue();
	};

	namespace Utils
	{
		inline void DumpGPUInfo()
		{
			auto & caps = Renderer::GetCapabilities();
			SE_CORE_INFO("Renderer", "GPU Info:");
			SE_CORE_INFO("Renderer", "Vendor: {0}", caps.Vendor);
			SE_CORE_INFO("Renderer", "Device: {0}", caps.Device);
			SE_CORE_INFO("Renderer", "Version: {0}", caps.Version);
		}
}
