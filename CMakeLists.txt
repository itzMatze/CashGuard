cmake_minimum_required(VERSION 3.19)
project(CashGuard)
set(CMAKE_CXX_STANDARD 20)

set(SPDLOG_DIR "${PROJECT_SOURCE_DIR}/dependencies/spdlog")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCE_FILES
	src/util/random_generator.cpp
	src/transaction.cpp
	include/transaction_model.hpp
	src/transaction_model.cpp
	include/table_style_delegate.hpp
	src/table_style_delegate.cpp)

add_executable(CashGuard src/main.cpp ${SOURCE_FILES}
	qt/mainwindow.h qt/mainwindow.cpp qt/mainwindow.ui
	qt/transactiondialog.h qt/transactiondialog.cpp)

target_include_directories(CashGuard PRIVATE
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/qt"
	"${PROJECT_SOURCE_DIR}/dependencies/"
	"${SPDLOG_DIR}/include")

add_subdirectory("${SPDLOG_DIR}")
target_link_libraries(CashGuard Qt6::Core Qt6::Widgets Qt6::Gui spdlog::spdlog)
