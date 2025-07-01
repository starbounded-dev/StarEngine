#include "sepch.h"
#include "Pipeline.h"

#include "Renderer.h"

#include "RendererAPI.h"

namespace StarEngine {

	namespace Utils {

		static nvrhi::Format GetNVRHIFormat(ShaderDataType type) {

			switch (type)
			{
			case ShaderDataType::Float:    return nvrhi::Format::R32_FLOAT;
			case ShaderDataType::Float2:   return nvrhi::Format::RG32_FLOAT;
			case ShaderDataType::Float3:   return nvrhi::Format::RGB32_FLOAT;
			case ShaderDataType::Float4:   return nvrhi::Format::RGBA32_FLOAT;
			case ShaderDataType::Int:     return nvrhi::Format::R8_SNORM;
			case ShaderDataType::Int2:    return nvrhi::Format::RG8_SNORM;
			case ShaderDataType::Int3:    return nvrhi::Format::RGBA8_SNORM;
			case ShaderDataType::Int4:    return nvrhi::Format::RGBA8_SNORM;
			case ShaderDataType::Bool:    return nvrhi::Format::RGBA32_FLOAT;
			}

			SE_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return nvrhi::Format::UNKNOWN;
		}

		static nvrhi::PrimitiveType GetNVRHIPrimitiveType(PrimitiveTolopogy topology)
		{
			switch (topology)
			{
				case PrimitiveTolopogy::Points:         return nvrhi::PrimitiveType::PointList;
				case PrimitiveTolopogy::Lines:          return nvrhi::PrimitiveType::LineList;
				case PrimitiveTolopogy::Triangles:      return nvrhi::PrimitiveType::TriangleList;
				case PrimitiveTolopogy::TriangleStrip:  return nvrhi::PrimitiveType::TriangleStrip;
				case PrimitiveTolopogy::TriangleFan:    return nvrhi::PrimitiveType::TriangleFan;
			}

			SE_CORE_ASSERT(false, "Unknown PrimitiveTolopogy!");
			return nvrhi::PrimitiveType::PointList; // Default fallback
		}

		static nvrhi::ComparisonFunc GetNVRHICompareOperator(const DepthCompareOperator compareOp)
		{
			switch (compareOp)
			{
			case DepthCompareOperator::Never:          return nvrhi::ComparisonFunc::Never;
			case DepthCompareOperator::NotEqual:       return nvrhi::ComparisonFunc::NotEqual;
			case DepthCompareOperator::Less:           return nvrhi::ComparisonFunc::Less;
			case DepthCompareOperator::LessOrEqual:    return nvrhi::ComparisonFunc::LessOrEqual;
			case DepthCompareOperator::Greater:        return nvrhi::ComparisonFunc::Greater;
			case DepthCompareOperator::GreaterOrEqual: return nvrhi::ComparisonFunc::GreaterOrEqual;
			case DepthCompareOperator::Equal:          return nvrhi::ComparisonFunc::Equal;
			case DepthCompareOperator::Always:         return nvrhi::ComparisonFunc::Always;
			}

			SE_CORE_ASSERT(false, "Unknown DepthCompareOperator!");
			return nvrhi::ComparisonFunc::Never; // Default fallback
		}
	}

	Pipeline::Pipeline(const PipelineSpecification& spec) : m_Specification(spec)
	{
		SE_PROFILE_FUNCTION();

		SE_CORE_ASSERT(spec.Shader);
		SE_CORE_ASSERT(spec.TargetFramebuffer);
		Invalidate();
		Renderer::RegisterShaderDependency(spec.Shader, this);
	}

	Pipeline::~Pipeline()
	{

	}

	void Pipeline::Invalidate() {
		Ref<Pipeline> instance = this;
		Renderer::Submit([instance]() mutable {
			instance->RT_Invalidate();
		});
	}

	void Pipeline::RT_Invalidate()
	{
		nvrhi::IDevice* device = Application::Get().GetWindow().GetDeviceManager()->GetDevice();
		Ref<Shader> shader = Ref<Shader>(m_Specification.Shader);
		Ref<Framebuffer> framebuffer = m_Specification.TargetFramebuffer->As<Framebuffer>();

		nvrhi::GraphicsPipelineDesc pipelineDesc;

#pragma region Shaders
		pipelineDesc.bindingLayouts = shader->GetAllDescriptorSetLayouts();

		const auto& shaderHandles = shader->GetHandles();
		if (shaderHandles.contains(nvrhi::ShaderType::Vertex))
			pipelineDesc.VS = shaderHandles.at(nvrhi::ShaderType::Vertex);
		if (shaderHandles.contains(nvrhi::ShaderType::Pixel))
			pipelineDesc.PS = shaderHandles.at(nvrhi::ShaderType::Pixel);

		pipelineDesc.primType = Utils::GetNVRHIPrimitiveType(m_Specification.Topology);
#pragma endregion

#pragma region RasterState
		nvrhi::RasterState& rasterState = pipelineDesc.renderState.rasterState;
		rasterState.cullMode = m_Specification.BackfaceCulling ? nvrhi::RasterCullMode::Back : nvrhi::RasterCullMode::None;
		rasterState.fillMode = m_Specification.Wireframe ? nvrhi::RasterFillMode::Line : nvrhi::RasterFillMode::Solid;
		rasterState.frontCounterClockwise = false;
		rasterState.multisampleEnable = false;
#pragma endregion

#pragma region DepthStenciState
		nvrhi::DepthStencilState& depthStencilState = pipelineDesc.renderState.depthStencilState;
		depthStencilState.depthTestEnable = m_Specification.DepthTest;
		depthStencilState.depthWriteEnable = m_Specification.DepthWrite;
		depthStencilState.depthFunc = Utils::GetNVRHICompareOperator(m_Specification.DepthCompareOperator);

		depthStencilState.stencilEnable = false;
		depthStencilState.backFaceStencil.failOp = nvrhi::StencilOp::Keep;
		depthStencilState.backFaceStencil.passOp = nvrhi::StencilOp::Keep;
		depthStencilState.backFaceStencil.stencilFunc = nvrhi::ComparisonFunc::Always;
		depthStencilState.frontFaceStencil.failOp = nvrhi::StencilOp::Keep;

#pragma endregion

#pragma region InputLayout
#pragma endregion
	}

	bool Pipeline::IsDynamicLineWidth() const
	{
		return m_Specification.Topology == PrimitiveTolopogy::Lines ||
			m_Specification.Topology == PrimitiveTolopogy::LineStrip ||
			m_Specification.Topology == PrimitiveTolopogy::TriangleStrip;
	}
}
