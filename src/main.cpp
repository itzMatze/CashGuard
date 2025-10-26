#include <vector>
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "util/log.hpp"
#include <QApplication>
#include "mainwindow.hpp"

#include "argparse/argparse.hpp"

int parse_args(int argc, char** argv, argparse::ArgumentParser& program)
{
	program.add_argument("--file").nargs(1).help("Override the .json file that is used by the application.");

	try
	{
		program.parse_args(argc, argv);
		if (program.get<bool>("--help"))
		{
			std::cout << program << std::endl;
			return 1;
		}
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return 1;
	}
	return 0;
}

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
#endif
	spdlog::set_level(spdlog::level::info);

	argparse::ArgumentParser program("CashGuard");
	int parse_status = parse_args(argc, argv, program);
	if (parse_status != 0) return parse_status;

	QApplication app(argc, argv);

	QString file_path = getenv("HOME");
	if (!file_path.endsWith("/")) file_path += "/";
	file_path += QString("Documents/CashGuardTransactions.json");

	if (program.is_used("--file")) file_path = QString::fromStdString(program.get<std::string>("--file"));
	MainWindow main_window;
	if (!main_window.init(file_path)) return 1;
	main_window.setWindowTitle("CashGuard");
	main_window.resize(800, 600);
	main_window.show();

	return app.exec();
}
