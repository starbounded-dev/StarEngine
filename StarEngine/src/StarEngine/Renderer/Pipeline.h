#pragma once

#include "StarEngine/Core/Ref.h"

#include "StarEngine/Renderer/VertexBuffer.h"
#include "StarEngine/Renderer/UniformBuffer.h"
#include "StarEngine/Renderer/Framebuffer.h"
#include "StarEngine/Renderer/Shader.h"

#include <nvrhi/nvrhi.h>

namespace StarEngine {

	enum class PrimitiveTolopogy {
		None = 0,
		Points,
		Lines,
		Triangles,
		LineStrip,
		TriangleStrip,
		TriangleFan
	};

	enum class DepthCompareOperator {
		None = 0,
		Never,
		NotEqual,
		Less,
		LessOrEqual,
		Greater,
		GreaterOrEqual,
		Equal,
		Always
	};

	struct PipelineSpecification {
		Ref<Shader> Shader;
		Ref<Framebuffer> TargetFramebuffer;
		VertexBufferLayout Layout;
		VertexBufferLayout InstanceLayout;
		VertexBufferLayout BoneInfluenceLayout;
		PrimitiveTolopogy Topology = PrimitiveTolopogy::Triangles;
		DepthCompareOperator DepthCompareOperator = DepthCompareOperator::GreaterOrEqual;
		bool BackfaceCulling = true;
		bool DepthTest = true;
		bool DepthWrite = true;
		bool Wireframe = false;
		float LineWidth = 1.0f;

		std::string DebugName;
	};

	struct PipelineStatistics {
		uint64_t InputAssemblyVertices = 0;
		uint64_t InputAssemblyPrimitives = 0;
		uint64_t VertexShaderInvocations = 0;
		uint64_t ClippingInvocations = 0;
		uint64_t ClippingPrimitives = 0;
		uint64_t FragmentShaderInvocations = 0;
		uint64_t ComputeShaderInvocations = 0;
	};

	enum class PipelineStage {
		None = 0,
		TopOfPipe = 0x00000001,
		DrawIndirect = 0x00000002,
		VertexInput = 0x00000004,
		VertexShader = 0x00000008,
		TessellationControlShader = 0x00000010,
		TessellationEvaluationShader = 0x00000020,
		GeometryShader = 0x00000040,
		FragmentShader = 0x00000080,
		EarlyFragmentTests = 0x00000100,
		LateFragmentTests = 0x00000200,
		ColorAttachmentOutput = 0x00000400,
		ComputeShader = 0x00000800,
		Transfer = 0x00001000,
		BottomOfPipe = 0x00002000,
		Host = 0x00004000,
		AllGraphics = 0x00008000,
		AllCommands = 0x00010000,
	};

	enum class ResourcesAccessFlags {
		None = 0,
		IndirectCommandRead = 0x00000001,
		IndexRead = 0x00000002,
		VertexAttributeRead = 0x00000004,
		UniformRead = 0x00000008,
		InputAttachmentRead = 0x00000010,
		ShaderRead = 0x00000020,
		ShaderWrite = 0x00000040,
		ColorAttachmentRead = 0x00000080,
		ColorAttachmentWrite = 0x00000100,
		DepthStencilAttachmentRead = 0x00000200,
		DepthStencilAttachmentWrite = 0x00000400,
		TransferRead = 0x00000800,
		TransferWrite = 0x00001000,
		HostRead = 0x00002000,
		HostWrite = 0x00004000,
		MemoryRead = 0x00008000,
		MemoryWrite = 0x00010000,
	};

	class Pipeline : public RefCounted
	{
	public:
		static Ref<Pipeline> Create(const PipelineSpecification& spec) { return Ref<Pipeline>::Create(spec); }

		PipelineSpecification& GetSpecification();
		const PipelineSpecification& GetSpecification() const;

		nvrhi::GraphicsPipelineHandle GetHandle() { return m_Handle; }

		void Invalidate();
		void RT_Invalidate();

		Ref<Shader> GetShader() const;

		bool IsDynamicLineWidth() const;
	public:
		Pipeline(const PipelineSpecification& spec);

		virtual ~Pipeline() = default;
	private:
		nvrhi::GraphicsPipelineHandle m_Handle = nullptr;
		PipelineSpecification m_Specification;
	};
}
