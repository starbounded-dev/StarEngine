#include "sepch.h"
#include "StarEngine/Renderer/VertexBuffer.h"

#include "StarEngine/Core/Application.h"

namespace StarEngine {

	VertexBuffer::VertexBuffer(const Buffer& buffer)
		: m_Size(buffer.Size)
	{
		// TODO: Render Thread?, if yes copy to m_LocalData

		auto vertexBufferDesc = nvrhi::BufferDesc()
			.setByteSize(buffer.Size)
			.setIsVertexBuffer(true)
			.setInitialState(nvrhi::ResourceStates::VertexBuffer)
			.setKeepInitialState(true)
			.setDebugName("VertexBuffer");

		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();
		m_Handle = device->createBuffer(vertexBufferDesc);

		nvrhi::CommandListHandle commandList = device->createCommandList();
		commandList->open();
		commandList->writeBuffer(m_Handle, buffer.Data, buffer.Size);
		commandList->close();
		device->executeCommandList(commandList);
	}

	VertexBuffer::VertexBuffer(uint64_t size)
		: m_Size(size)
	{
		m_LocalData.Allocate(size);

		auto vertexBufferDesc = nvrhi::BufferDesc()
			.setByteSize(size)
			.setIsVertexBuffer(true)
			.setInitialState(nvrhi::ResourceStates::VertexBuffer)
			.setKeepInitialState(true)
			.setDebugName("VertexBuffer");

		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();
		m_Handle = device->createBuffer(vertexBufferDesc);
	}

	void VertexBuffer::SetData(Buffer buffer, uint64_t offset) 
	{
		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();
		nvrhi::CommandListHandle commandList = device->createCommandList();
		commandList->open();
		commandList->writeBuffer(m_Handle, buffer.Data, buffer.Size);
		commandList->close();
		device->executeCommandList(commandList);
	}

	void VertexBuffer::RT_SetData(Buffer buffer, uint64_t offset)
	{
		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();
		nvrhi::CommandListHandle commandList = device->createCommandList();
		commandList->open();
		commandList->writeBuffer(m_Handle, buffer.Data, buffer.Size);
		commandList->close();
		device->executeCommandList(commandList);
	}
};
