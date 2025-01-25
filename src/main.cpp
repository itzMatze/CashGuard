#include <vector>
#include "util/log.hpp"
#include <QApplication>
#include "mainwindow.hpp"

#include "argparse/argparse.hpp"

int parse_args(int argc, char** argv, argparse::ArgumentParser& program)
{
  program.add_argument("--file").nargs(1).help("Override the .json file that is loaded on startup and saved to on shutdown");

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
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
#ifndef NDEBUG
  sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>("CashGuard.log", true));
#endif
  auto combined_logger = std::make_shared<spdlog::logger>("default_logger", sinks.begin(), sinks.end());
  spdlog::set_default_logger(combined_logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%Y-%m-%d %T.%e] [%L] %v");

  argparse::ArgumentParser program("CashGuard");
  int parse_status = parse_args(argc, argv, program);
  if (parse_status != 0) return parse_status;

	QApplication app(argc, argv);

	QString filePath = getenv("HOME");
	if (!filePath.endsWith("/")) filePath += "/";
	filePath += QString("Documents/Stuff/CashGuardTransactions.json");

	if (program.is_used("--file")) filePath = QString::fromStdString(program.get<std::string>("--file"));
	MainWindow mainWindow(filePath);
	mainWindow.setWindowTitle("CashGuard");
	mainWindow.resize(800, 600);
	mainWindow.show();

	return app.exec();
}
