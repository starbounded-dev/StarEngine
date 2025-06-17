#include "sepch.h"
#include "IndexBuffer.h"

#include "StarEngine/Core/Application.h"
#include "StarEngine/Core/Buffer.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine {

	IndexBuffer::IndexBuffer(const Buffer& buffer)
		: m_Size(buffer.Size)
	{
		// TODO: Render Thread?, if yes copy to m_LocalData

		auto indexBufferDesc = nvrhi::BufferDesc()
			.setByteSize(buffer.Size)
			.setIsVertexBuffer(true)
			.setInitialState(nvrhi::ResourceStates::IndexBuffer)
			.setKeepInitialState(true)
			.setDebugName("IndexBuffer");

		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();
		m_Handle = device->createBuffer(indexBufferDesc);

		nvrhi::CommandListHandle commandList = device->createCommandList();
		commandList->open();
		commandList->writeBuffer(m_Handle, buffer.Data, buffer.Size);
		commandList->close();
		device->executeCommandList(commandList);
	}

	IndexBuffer::IndexBuffer(uint64_t size)
		: m_Size(size)
	{
		auto indexBufferDesc = nvrhi::BufferDesc()
			.setByteSize(size)
			.setIsVertexBuffer(true)
			.setInitialState(nvrhi::ResourceStates::IndexBuffer)
			.setKeepInitialState(true)
			.setDebugName("IndexBuffer");

		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();
		m_Handle = device->createBuffer(indexBufferDesc);
	}

	void IndexBuffer::SetData(void* buffer, uint64_t size, uint64_t offset) {
		SE_CORE_VERIFY(false, "Not implemented.")
	}
}
