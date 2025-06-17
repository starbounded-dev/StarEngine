#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <nvrhi/nvrhi.h>

#include <imgui.h>

namespace StarEngine
{
	struct ImGuiRenderer
	{
		nvrhi::DeviceHandle m_device;
		nvrhi::CommandListHandle m_commandList;

		nvrhi::ShaderHandle vertexShader;
		nvrhi::ShaderHandle pixelShader;
		nvrhi::InputLayoutHandle shaderAttribLayout;

		nvrhi::TextureHandle fontTexture;
		nvrhi::SamplerHandle fontSampler;

		nvrhi::BufferHandle vertexBuffer;
		nvrhi::BufferHandle indexBuffer;

		nvrhi::BindingLayoutHandle bindingLayout;
		nvrhi::GraphicsPipelineDesc basePSODesc;

		nvrhi::GraphicsPipelineHandle pso;
		std::unordered_map<nvrhi::ITexture*, nvrhi::BindingSetHandle> bindingsCache;

		std::vector<ImDrawVert> vtxBuffer;
		std::vector<ImDrawIdx> idxBuffer;

		bool Init(nvrhi::IDevice* device);
		bool UpdateFontTexture();
		bool Render(nvrhi::IFramebuffer* framebuffer);
		void BackbufferResizing();

	private:
		bool ReallocateBuffer(nvrhi::BufferHandle& buffer, size_t requiredSize, size_t reallocateSize, bool isIndexBuffer);


		nvrhi::IGraphicsPipeline* getPSO(nvrhi::IFramebuffer* fb);
		nvrhi::IBindingSet* getBindingSet(nvrhi::ITexture* texture);
		bool updateGeometry(nvrhi::ICommandList* commandList);
	};
}
