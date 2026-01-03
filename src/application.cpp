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

	if (!cg_file_handler.load_from_file(file_path, transaction_model, account_model, category_model))
	{
		cglog::error("Failed to load file \"{}\". Exiting.", file_path);
		return false;
	}
	return true;
}

int32_t Application::run()
{
	Window window;
	if (!window.construct("CashGuard")) return 1;
	ui.init(transaction_model, account_model, category_model);
	bool quit = false;
	SDL_Event event;
	while (!quit)
	{
		window.new_frame();
		ImVec2 available_space = ImGui::GetContentRegionAvail();
		ui.draw(available_space, transaction_model, account_model, category_model);
		window.end_frame();
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) quit = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == window.get_id()) quit = true;
		}
		if (transaction_model.dirty || account_model.dirty || category_model.dirty)
		{
			if (!cg_file_handler.save_to_file(file_path, transaction_model, account_model, category_model)) cglog::error("Failed to save file \"{}\".", file_path);
			transaction_model.dirty = false;
			account_model.dirty = false;
			category_model.dirty = false;
		}
	}
	window.destruct();
	return 0;
}
