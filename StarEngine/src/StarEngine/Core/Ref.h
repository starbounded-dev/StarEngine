#pragma once

#include <stdint.h>
#include <atomic>

namespace StarEngine {

	class RefCounted
	{
	public:
		virtual ~RefCounted() = default;

		void IncRefCount() const
		{
			++m_RefCount;
		}
		void DecRefCount() const
		{
			--m_RefCount;
		}

		uint32_t GetRefCount() const { return m_RefCount.load(); }
	private:
		mutable std::atomic<uint32_t> m_RefCount = 0;
	};

	namespace RefUtils {
		void AddToLiveReferences(void* instance);
		void RemoveFromLiveReferences(void* instance);
		bool IsLive(void* instance);
	}

	template<typename T>
	class RefPtr
	{
	public:
		RefPtr()
			: m_Instance(nullptr)
		{
		}

		RefPtr(std::nullptr_t n)
			: m_Instance(nullptr)
		{
		}

		RefPtr(T* instance)
			: m_Instance(instance)
		{
			static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted!");

			IncRef();
		}

		template<typename T2>
		RefPtr(const RefPtr<T2>& other)
		{
			m_Instance = (T*)other.m_Instance;
			IncRef();
		}

		template<typename T2>
		RefPtr(RefPtr<T2>&& other)
		{
			m_Instance = (T*)other.m_Instance;
			other.m_Instance = nullptr;
		}

		static RefPtr<T> CopyWithoutIncrement(const RefPtr<T>& other)
		{
			RefPtr<T> result = nullptr;
			result->m_Instance = other.m_Instance;
			return result;
		}

		~RefPtr()
		{
			DecRef();
		}

		RefPtr(const RefPtr<T>& other)
			: m_Instance(other.m_Instance)
		{
			IncRef();
		}

		RefPtr& operator=(std::nullptr_t)
		{
			DecRef();
			m_Instance = nullptr;
			return *this;
		}

		RefPtr& operator=(const RefPtr<T>& other)
		{
			if (this == &other)
				return *this;

			other.IncRef();
			DecRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		RefPtr& operator=(const RefPtr<T2>& other)
		{
			other.IncRef();
			DecRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		RefPtr& operator=(RefPtr<T2>&& other)
		{
			DecRef();

			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* Raw() { return  m_Instance; }
		const T* Raw() const { return  m_Instance; }

		void Reset(T* instance = nullptr)
		{
			DecRef();
			m_Instance = instance;
		}

		template<typename T2>
		RefPtr<T2> As() const
		{
			return RefPtr<T2>(*this);
		}

		template<typename... Args>
		static RefPtr<T> Create(Args&&... args)
		{
			return RefPtr<T>(new T(std::forward<Args>(args)...));
		}

		bool operator==(const RefPtr<T>& other) const
		{
			return m_Instance == other.m_Instance;
		}

		bool operator!=(const RefPtr<T>& other) const
		{
			return !(*this == other);
		}

		bool EqualsObject(const RefPtr<T>& other)
		{
			if (!m_Instance || !other.m_Instance)
				return false;

			return *m_Instance == *other.m_Instance;
		}
	private:
		void IncRef() const
		{
			if (m_Instance)
			{
				m_Instance->IncRefCount();
			}
		}

		void DecRef() const
		{
			if (m_Instance)
			{
				m_Instance->DecRefCount();

				if (m_Instance->GetRefCount() == 0)
				{
					delete m_Instance;
					m_Instance = nullptr;
				}
			}
		}

		template<class T2>
		friend class RefPtr;
		mutable T* m_Instance;
	};

	template<typename T>
	class WeakRef
	{
	public:
		WeakRef() = default;

		WeakRef(RefPtr<T> ref)
		{
			m_Instance = ref.Raw();
		}

		WeakRef(T* instance)
		{
			m_Instance = instance;
		}

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		bool IsValid() const { return m_Instance != nullptr; }
		operator bool() const { return IsValid(); }

		template<typename T2>
		WeakRef<T2> As() const
		{
			return WeakRef<T2>(dynamic_cast<T2*>(m_Instance));
		}
	private:
		T* m_Instance = nullptr;
	};

}
