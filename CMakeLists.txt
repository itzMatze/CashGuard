cmake_minimum_required(VERSION 3.19)
project(CashGuard)
set(CMAKE_CXX_STANDARD 20)

set(SPDLOG_DIR "${PROJECT_SOURCE_DIR}/dependencies/spdlog")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(SOURCE_FILES )

add_executable(CashGuard src/main.cpp ${SOURCE_FILES})

target_include_directories(CashGuard PRIVATE "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/dependencies/"
  "${SPDLOG_DIR}/include")

add_subdirectory("${SPDLOG_DIR}")
target_link_libraries(CashGuard Qt6::Core Qt6::Widgets Qt6::Gui spdlog::spdlog)
