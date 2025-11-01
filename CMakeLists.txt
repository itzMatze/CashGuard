cmake_minimum_required(VERSION 3.19)
project(CashGuard)
set(CMAKE_CXX_STANDARD 20)

set(SPDLOG_DIR "${PROJECT_SOURCE_DIR}/dependencies/spdlog")
add_subdirectory("${SPDLOG_DIR}")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Charts)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCE_FILES
	include/mainwindow.hpp
	src/mainwindow.cpp
	src/mainwindow_ui.cpp
	include/tooltip_chart_view.hpp
	src/tooltip_chart_view.cpp
	# account
	include/account_dialog.hpp
	src/account_dialog.cpp
	include/account_model.hpp
	src/account_model.cpp
	# transaction dialogs
	include/transaction/transaction_dialog.hpp
	src/transaction/transaction_dialog.cpp
	src/transaction/transaction_dialog_ui.cpp
	include/transaction/transaction_group_dialog.hpp
	src/transaction/transaction_group_dialog.cpp
	src/transaction/transaction_group_dialog_ui.cpp
	# transaction filter
	include/transaction_filter/transaction_filter_window.hpp
	src/transaction_filter/transaction_filter.cpp
	src/transaction_filter/transaction_filter_window.cpp
	src/transaction_filter/transaction_filter_window_ui.cpp
	# transaction
	include/transaction_model.hpp
	src/transaction_model.cpp
	src/transaction.cpp
	src/transaction_file_handler.cpp
	include/transaction_table_style_delegate.hpp
	src/transaction_table_style_delegate.cpp
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
