#include "sepch.h"
#include "Platform.h"

#include <chrono>

namespace StarEngine {

	uint64_t Platform::GetCurrentDateTimeU64()
	{
		std::string string = GetCurrentDateTimeString();
		return std::stoull(string);
	}

	std::string Platform::GetCurrentDateTimeString()
	{
		auto now = std::chrono::system_clock::now();
		return std::format("{:%Y%m%d%H%M}", now);
	}
}
