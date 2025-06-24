#pragma once

/**
 * Represents a universally unique identifier (UUID) as a 64-bit unsigned integer.
 *
 * Provides constructors for default and explicit initialization, as well as conversion operators to retrieve the underlying value.
 */
namespace StarEngine {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() { return m_UUID; }
		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

/**
 * Computes a hash value for a StarEngine::UUID.
 * @return The 64-bit integer value of the UUID as its hash code.
 */
namespace std {

	//template <typename T> struct hash;

	template<>
	struct hash<StarEngine::UUID>
	{
		std::size_t operator()(const StarEngine::UUID& uuid) const
		{
			return uuid;
			//return (uint64_t)uuid;
		}
	};

}
