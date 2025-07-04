cmake_minimum_required(VERSION 3.19)
project(CashGuard)
set(CMAKE_CXX_STANDARD 20)

set(SPDLOG_DIR "${PROJECT_SOURCE_DIR}/dependencies/spdlog")
add_subdirectory("${SPDLOG_DIR}")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Charts)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCE_FILES
	include/table_style_delegate.hpp
	include/mainwindow.hpp
	src/mainwindow.cpp
	include/tooltip_chart_view.hpp
	src/tooltip_chart_view.cpp
	include/transaction_ui/transaction_dialog.hpp
	src/transaction_ui/transaction_dialog.cpp
	include/transaction_filter_ui/transaction_filter_window.hpp
	src/transaction_filter_ui/transaction_filter_window.cpp
	src/transaction_filter_ui/transaction_filter.cpp
	include/transaction_ui/transaction_group_dialog.hpp
	src/transaction_ui/transaction_group_dialog.cpp
	include/transaction_model.hpp
	src/transaction_model.cpp
	src/transaction.cpp
	src/transaction_file_handler.cpp
	src/table_style_delegate.cpp
	src/total_amount.cpp
	src/validation.cpp
	src/util/random_generator.cpp
)

add_executable(CashGuard src/main.cpp ${SOURCE_FILES})

target_include_directories(CashGuard PRIVATE
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/dependencies/"
	"${SPDLOG_DIR}/include")

target_link_libraries(CashGuard Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts spdlog::spdlog)
