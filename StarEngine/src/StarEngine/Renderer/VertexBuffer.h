#pragma once

#include "StarEngine/Core/Core.h"
#include "StarEngine/Core/Ref.h"
#include "StarEngine/Core/Buffer.h"

#include <nvrhi/nvrhi.h>

namespace StarEngine {

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;

		}

		SE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement {
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {

		}

		uint32_t GetComponentCount() const {
			switch (Type)
			{
			case ShaderDataType::Float:    return 1;
			case ShaderDataType::Float2:   return 2;
			case ShaderDataType::Float3:   return 3;
			case ShaderDataType::Float4:   return 4;
			case ShaderDataType::Mat3:	   return 3; // 3* float3
			case ShaderDataType::Mat4:	   return 4; // 4* float4
			case ShaderDataType::Int:      return 1;
			case ShaderDataType::Int2:     return 2;
			case ShaderDataType::Int3:     return 3;
			case ShaderDataType::Int4:     return 4;
			case ShaderDataType::Bool:     return 1;
			}

			SE_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}


	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(std::initializer_list<BufferElement> elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;

			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer : public RefCounted
	{
	public:
		static RefPtr<VertexBuffer> Create(const Buffer& buffer) {
			return std::make_shared<VertexBuffer>(buffer);
		}

		static RefPtr<VertexBuffer> Create(uint64_t size) {
			return std::make_shared<VertexBuffer>(size);
		}

		void SetData(Buffer buffer, uint64_t offset = 0);
		void RT_SetData(Buffer buffer, uint64_t offset = 0);

		uint64_t GetSize() const { return m_Size; }
		nvrhi::BufferHandle GetHandle() const { return m_Handle; }

	public:
		VertexBuffer(const Buffer& buffer);
		VertexBuffer(uint64_t size);

		virtual ~VertexBuffer() = default;

	private:
		nvrhi::BufferHandle m_Handle;
		uint64_t m_Size = 0;
		Buffer m_LocalData;
	};
}
