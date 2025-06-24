#pragma once
 
#include <cstdint>
#include <string_view>
#include <string>

/**
	 * Computes a 32-bit FNV-1a hash of the given string.
	 * @param str The input string to hash.
	 * @return The 32-bit FNV-1a hash value.
	 */
	
	/**
	 * Computes a 64-bit FNV-1a hash of the given string.
	 * @param str The input string to hash.
	 * @return The 64-bit FNV-1a hash value.
	 */
	namespace StarEngine {

	class Hash
	{
	public:
		static constexpr uint32_t GenerateFNVHash(std::string_view str)
		{
			constexpr uint32_t FNV_PRIME = 16777619u;
			constexpr uint32_t OFFSET_BASIS = 2166136261u;

			const size_t length = str.length();
			const char* data = str.data();

			uint32_t hash = OFFSET_BASIS;
			for (size_t i = 0; i < length; ++i)
			{
				hash ^= *data++;
				hash *= FNV_PRIME;
			}
			hash ^= '\0';
			hash *= FNV_PRIME;

			return hash;
		}

		static constexpr uint64_t GenerateFNVHash64Bit(std::string_view str)
		{
			//constexpr uint64_t FNV_PRIME = 16777619u;
			//constexpr uint64_t OFFSET_BASIS = 2166136261u;
			constexpr uint64_t FNV_PRIME = 1099511628211u;
			constexpr uint64_t OFFSET_BASIS = 14695981039346656037ull;

			const size_t length = str.length();
			const char* data = str.data();

			uint64_t hash = OFFSET_BASIS;
			for (size_t i = 0; i < length; ++i)
			{
				hash ^= *data++;
				hash *= FNV_PRIME;
			}
			hash ^= '\0';
			hash *= FNV_PRIME;

			return hash;
		}
	};

}