#pragma once

#include <stdint.h>
#include <cstring>
#include <memory>

/**
		 * Represents a non-owning raw byte buffer for storage and manipulation of arbitrary data.
		 *
		 * Provides methods for allocation, release, zero-initialization, reading and writing typed data at specified offsets, and copying buffer contents. 
		 * The buffer does not manage ownership unless explicitly allocated or released.
		 *
		 * @param Data Pointer to the raw byte data.
		 * @param Size Size of the buffer in bytes.
		 */
		namespace StarEngine {

	// Non-owning raw buffer class
	struct Buffer
	{
		uint8_t* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;

		Buffer(uint64_t size)
		{
			Allocate(size);
		}

		Buffer(const void* data, uint64_t size)
			: Data((uint8_t*)data), Size(size)
		{
		}

		Buffer(const Buffer&) = default;

		static Buffer Copy(Buffer other)
		{
			Buffer result(other.Size);
			memcpy(result.Data, other.Data, other.Size);

			return result;
		}

		void Allocate(uint64_t size)
		{
			Release();

			Data = (uint8_t*)malloc(size);
			Size = size;
		}

		void Release()
		{
			free(Data);
			Data = nullptr;
			Size = 0;
		}

		void ZeroInitialize()
		{
			if (Data)
				memset(Data, 0, Size);
		}

		template<typename T>
		T& Read(uint64_t offset = 0)
		{
			return *(T*)((byte*)Data + offset);
		}

		template<typename T>
		const T& Read(uint64_t offset = 0) const
		{
			return *(T*)((byte*)Data + offset);
		}

		std::unique_ptr<uint8_t[]> ReadBytes(uint32_t size, uint32_t offset)
		{
			SE_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
			std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(size);
			memcpy(buffer.get(), (uint8_t*)Data + offset, size);
			return buffer;
		}

		void Write(const void* data, uint64_t size, uint64_t offset = 0)
		{
			SE_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
			memcpy((uint8_t*)Data + offset, data, size);
		}

		operator bool() const
		{
			return (bool)Data;
		}

		template<typename T>
		T* As() const
		{
			return (T*)Data;
		}
	};

	/**
	 * A buffer that automatically releases its memory when destroyed.
	 *
	 * Inherits from Buffer and ensures memory is freed on destruction.
	 * Provides a static method to create a buffer by copying data from a given source.
	 */
	struct BufferSafe : public Buffer
	{
		~BufferSafe()
		{
			Release();
		}

		static BufferSafe Copy(const void* data, uint64_t size)
		{
			BufferSafe buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}
	};

	/**
		 * Returns a pointer to the underlying buffer's data as a `uint8_t*`.
		 * @returns Pointer to the buffer's data, or `nullptr` if the buffer is empty.
		 */
		struct ScopedBuffer
	{
		ScopedBuffer(Buffer buffer)
			: m_Buffer(buffer)
		{
		}

		ScopedBuffer(uint64_t size)
			: m_Buffer(size)
		{
		}

		~ScopedBuffer()
		{
			m_Buffer.Release();
		}

		uint8_t* Data() { return (uint8_t*)m_Buffer.Data; }
		/**
 * Returns the size of the underlying buffer in bytes.
 * @returns The number of bytes in the buffer.
 */
uint64_t Size() { return m_Buffer.Size; }

		template<typename T>
		T* As()
		{
			return m_Buffer.As<T>();
		}

		operator bool() const { return m_Buffer; }

	private:
		Buffer m_Buffer;
	};

}
