#include <vector>
#include "util/log.hpp"
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char** argv)
{
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
  sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>("CashGuard.log", true));
  auto combined_logger = std::make_shared<spdlog::logger>("default_logger", sinks.begin(), sinks.end());
  spdlog::set_default_logger(combined_logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%Y-%m-%d %T.%e] [%L] %v");

	QApplication app(argc, argv);

	MainWindow mainWindow;
	mainWindow.setWindowTitle("CashGuard");
	mainWindow.resize(800, 600);
	mainWindow.show();

	return app.exec();
}
