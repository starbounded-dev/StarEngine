#include "sepch.h"

#include "StarEngine/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace StarEngine
{
	RefPtr<spdlog::logger> Log::s_CoreLogger;
	RefPtr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("StarEngine.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_CoreLogger = std::make_shared<spdlog::logger>("STARENGINE", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
	}

	void Log::LogMessage(LogSeverity severity, const std::string& message, bool core)
	{
		auto& logger = core ? GetCoreLogger() : GetClientLogger();
		switch (severity)
		{
		case LogSeverity::Trace:    logger->trace(message); break;
		case LogSeverity::Info:     logger->info(message); break;
		case LogSeverity::Warn:     logger->warn(message); break;
		case LogSeverity::Error:    logger->error(message); break;
		case LogSeverity::Critical: logger->critical(message); break;
		}
	}
}
