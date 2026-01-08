#include "application.hpp"

#include "SDL3/SDL_events.h"
#include "backends/imgui_impl_sdl3.h"
#include "portable-file-dialogs.h"
#include "util/log.hpp"
#include "window.hpp"
#include <filesystem>
#include <fstream>

Application::Application(std::filesystem::path pref_path)
{
	cache_file_path = pref_path / "cache";
	if (!std::filesystem::exists(cache_file_path))
	{
		cglog::debug("Didn't find an existing cache file.");
		return;
	}
	std::ifstream cache_file(cache_file_path, std::ios::binary);
	if (!cache_file.is_open())
	{
		cglog::debug("Failed to open cache file for reading!");
		return;
	}
	std::string file_path;
	std::getline(cache_file, file_path);
	cache_file.close();
	if (!load_file(file_path, false))
	{
		cglog::debug("Failed to open file from cache!");
	}
}

Application::~Application()
{
	std::ofstream cache_file(cache_file_path, std::ios::binary | std::ios::trunc);
	if (!cache_file.is_open())
	{
		cglog::debug("Failed to open cache file for writing!");
		return;
	}
	cache_file << file_path.string() << "\n";
	cache_file.close();
	if (!file_path_valid) return;
}

bool Application::load_file(const std::string& file_path, bool create)
{
	std::filesystem::path file(file_path);
	if (create)
	{
		if (std::filesystem::exists(file))
		{
			cglog::error("Failed to create file \"{}\". It already exists.", file_path);
			return false;
		}
		if (file.has_parent_path() && !std::filesystem::exists(file.parent_path()))
		{
			bool success = std::filesystem::create_directories(file.parent_path());
			if (!success)
			{
				cglog::error("Failed to create directories!");
				return false;
			}
		}
		std::ofstream out_file(file);
		if (!out_file.is_open())
		{
			cglog::error("Failed to create file!");
			return false;
		}
		out_file.close();
	}
	if (!cg_file_handler.load_from_file(file, transaction_model, account_model, category_model)) return false;
	this->file_path = std::filesystem::absolute(file);
	file_path_valid = true;
	return true;
}

bool Application::prompt_new_file()
{
	pfd::save_file file = pfd::save_file("Select directory and filename to create", pfd::path::home(), { "CGT File", "*.cgt" }, pfd::opt::none);
	if (file.result().empty()) return false;
	std::filesystem::path path(file.result());
	if (path.extension() != ".cgt") path.replace_extension(".cgt");
	return load_file(path, true);
}

bool Application::prompt_open_file()
{
	pfd::open_file file = pfd::open_file("Select file to open", pfd::path::home(), { "CGT File", "*.cgt" }, pfd::opt::none);
	if (file.result().empty()) return false;
	std::filesystem::path path(file.result().at(0));
	if (path.extension() != ".cgt") path.replace_extension(".cgt");
	return load_file(path, false);
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
		if (ui.create_new_file)
		{
			ui.create_new_file = false;
			if (!prompt_new_file()) cglog::debug("Failed to create new file!");
			ui.init(transaction_model, account_model, category_model);
		}
		if (ui.open_existing_file)
		{
			ui.open_existing_file = false;
			if (!prompt_open_file()) cglog::debug("Failed to open file!");
			ui.init(transaction_model, account_model, category_model);
		}
		window.new_frame();
		ImVec2 available_space = ImGui::GetContentRegionAvail();
		ui.draw(available_space, transaction_model, account_model, category_model, file_path_valid);
		window.end_frame();
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) quit = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == window.get_id()) quit = true;
		}
		if (transaction_model.dirty || account_model.dirty || category_model.dirty)
		{
			if (!cg_file_handler.save_to_file(file_path, transaction_model, account_model, category_model)) cglog::error("Failed to save file \"{}\".", file_path.string());
			transaction_model.dirty = false;
			account_model.dirty = false;
			category_model.dirty = false;
		}
	}
	window.destruct();
	return 0;
}
