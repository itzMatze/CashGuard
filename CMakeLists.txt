cmake_minimum_required(VERSION 3.19)
project(CashGuard)
set(CMAKE_CXX_STANDARD 23)

set(SDL3_DIR "${PROJECT_SOURCE_DIR}/dependencies/SDL3")
set(SPDLOG_DIR "${PROJECT_SOURCE_DIR}/dependencies/spdlog")

add_subdirectory("${SDL3_DIR}")
add_subdirectory("${SPDLOG_DIR}")

find_package(OpenGL REQUIRED)

set(SOURCE_FILES
	src/account_model.cpp
	src/application.cpp
	src/category_model.cpp
	src/cg_file_handler.cpp
	src/dialogs.cpp
	src/graphs.cpp
	src/inputs.cpp
	src/transaction.cpp
	src/transaction_filter.cpp
	src/transaction_model.cpp
	src/transaction_table.cpp
	src/ui.cpp
	src/window.cpp
	src/util/color.cpp
	src/util/random_generator.cpp
	"${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui.cpp"
	"${PROJECT_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_sdl3.cpp"
	"${PROJECT_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_opengl3.cpp"
	"${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp"
	"${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_widgets.cpp"
	"${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_tables.cpp"
	"${PROJECT_SOURCE_DIR}/dependencies/implot/implot.cpp"
	"${PROJECT_SOURCE_DIR}/dependencies/implot/implot_items.cpp"
)

add_executable(CashGuard src/main.cpp ${SOURCE_FILES})

target_include_directories(CashGuard PRIVATE
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/dependencies/"
	"${PROJECT_SOURCE_DIR}/dependencies/imgui/"
	"${PROJECT_SOURCE_DIR}/dependencies/implot/"
	"${SDL3_DIR}/include/"
	"${SPDLOG_DIR}/include")

target_link_libraries(CashGuard
	OpenGL::GL
	SDL3::SDL3
	spdlog::spdlog
)
