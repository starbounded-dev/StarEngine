#pragma once

#include <chrono>
#include <unordered_map>

#include "Log.h"

namespace StarEngine {

	class Timer
	{
	public:
		Timer()
		{
			Reset();
		}

		void Reset()
		{
			m_Start = std::chrono::steady_clock::now();
		}

		using Microseconds = std::chrono::microseconds;
		using Milliseconds = std::chrono::milliseconds;
		using Seconds = std::chrono::seconds;

		template<typename T = Microseconds>
		float Elapsed()
		{
			return static_cast<float>(std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - m_Start).count());
		}

		float ElapsedMicros()
		{
			return Elapsed<Microseconds>() * 0.001f * 0.001f;
		}

		float ElapsedMillis()
		{
			return Elapsed<Milliseconds>() * 0.001f;
		}

		float ElapsedSecs()
		{
			return Elapsed<Seconds>();
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_Start;
	};

	/*struct ProfilingResult
	{
		const char* Name;
		float Time;
	};

	template<typename Fn>
	class ProfilingTimer
	{
	public:
		ProfilingTimer(const char* name, Fn&& func)
			: m_Name(name), m_Func(func), m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::steady_clock::now();
		}

		~ProfilingTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::steady_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			m_Stopped = true;

			float duration = (end - start) * 0.001f;
			m_Func({ m_Name, duration });
		}

	private:
		const char* m_Name;
		Fn m_Func;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
	};

#define PROFILE_SCOPE(name) ProfilingTimer timer##__LINE__(name, [&](ProfilingResult profileResult) { m_ProfileResults.push_back(profileResult); })*/

	class ScopedTimer
	{
	public:
		ScopedTimer(const std::string& name)
			: m_Name(name) {
		}
		~ScopedTimer()
		{
			float time = m_Timer.ElapsedMillis();
		}
	private:
		std::string m_Name;
		Timer m_Timer;
	};

	class PerformanceProfiler
	{
	public:
		struct PerFrameData
		{
			float Time = 0.0f;
			uint32_t Samples = 0;

			PerFrameData() = default;
			PerFrameData(float time) : Time(time) {}

			operator float() const { return Time; }
			inline PerFrameData& operator+=(float time)
			{
				Time += time;
			}
		};
	public:
		void SetPerFrameTiming(const char* name, float time)
		{
			std::scoped_lock<std::mutex> lock(m_PerFrameDataMutex);

			if (m_PerFrameData.find(name) == m_PerFrameData.end())
				m_PerFrameData[name] = 0.0f;

			PerFrameData& data = m_PerFrameData[name];
			data.Time += time;
			data.Samples++;
		}

		void Clear()
		{
			std::scoped_lock<std::mutex> lock(m_PerFrameDataMutex);
			m_PerFrameData.clear();
		}

		const std::unordered_map<const char*, PerFrameData>& GetPerFrameData() const { return m_PerFrameData; }
	private:
		std::unordered_map<const char*, PerFrameData> m_PerFrameData;
		inline static std::mutex m_PerFrameDataMutex;
	};

	class ScopePerfTimer
	{
	public:
		ScopePerfTimer(const char* name, PerformanceProfiler* profiler)
			: m_Name(name), m_Profiler(profiler) {
		}

		~ScopePerfTimer()
		{
			float time = m_Timer.ElapsedMillis();
			m_Profiler->SetPerFrameTiming(m_Name, time);
		}
	private:
		const char* m_Name;
		PerformanceProfiler* m_Profiler;
		Timer m_Timer;
	};

#if 1
#define SE_SCOPE_PERF(name)\
	ScopePerfTimer timer__LINE__(name, Application::Get().GetPerformanceProfiler());

#define SE_SCOPE_TIMER(name)\
	ScopedTimer timer__LINE__(name);
#else
#define SE_SCOPE_PERF(name)
#define SE_SCOPE_TIMER(name)
#endif

}
