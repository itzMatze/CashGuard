#include "application.hpp"

#include "SDL3/SDL_events.h"
#include "backends/imgui_impl_sdl3.h"
#include "util/log.hpp"
#include "window.hpp"
#include <filesystem>
#include <fstream>

bool Application::init(const std::string& file_path)
{
	this->file_path = file_path;
	if (!std::filesystem::exists(file_path))
	{
		cglog::info("Failed to find file \"{}\". Creating new file.", file_path);
		bool success = std::filesystem::create_directories(file_path);
		CG_ASSERT(success, "Failed to create directories!");
		std::ifstream file(file_path);
		CG_ASSERT(file.is_open(), "Failed to open file!");
		file.close();
	}

	if (!cg_file_handler.load_from_file(file_path, transaction_model, account_model))
	{
		cglog::error("Failed to load file \"{}\". Exiting.", file_path);
		return false;
	}
	if (!transaction_model.is_empty())
	{
		transaction_model.get_filter().date_max = transaction_model.get_unfiltered_transactions().at(0)->date;
		transaction_model.get_filter().date_min = transaction_model.get_unfiltered_transactions().back()->date;
	}
	return true;
}

int32_t Application::run()
{
	Window window;
	if (!window.construct("CashGuard")) return 1;
	bool quit = false;
	SDL_Event event;
	while (!quit)
	{
		window.new_frame();
		ImVec2 available_space = ImGui::GetContentRegionAvail();
		available_space.y -= ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemInnerSpacing.y * 2.0f;
		transaction_page.draw(available_space, transaction_model, account_model);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		window.end_frame();
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) quit = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == window.get_id()) quit = true;
		}
	}
	window.destruct();
	if (!cg_file_handler.save_to_file(file_path, transaction_model, account_model))
	{
		cglog::error("Failed to save file \"{}\".", file_path);
		return 1;
	}
	return 0;
}
