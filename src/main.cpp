#include <filesystem>
#include <vector>
#include "application.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "util/log.hpp"

int main(int argc, char** argv)
{
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stderr_color_sink_mt>());
	sinks[0]->set_pattern("%^[%Y-%m-%d %T.%e] [%L]%$ %v");
#ifndef NDEBUG
	sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>("CashGuard.log", true));
	sinks[1]->set_pattern("[%Y-%m-%d %T.%e] [%L] %v");
#endif
	std::shared_ptr<spdlog::logger> combined_logger = std::make_shared<spdlog::logger>("default_logger", sinks.begin(), sinks.end());
	spdlog::set_default_logger(combined_logger);
#ifndef NDEBUG
	spdlog::set_level(spdlog::level::debug);
#else
	spdlog::set_level(spdlog::level::info);
#endif

	cglog::info("Starting");
	Application app;
	return app.run();
}
