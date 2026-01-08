#pragma once
#include "category_model.hpp"
#include "cg_file_handler.hpp"
#include "ui.hpp"
#include "transaction_model.hpp"
#include <filesystem>
#include <string>

class Application
{
public:
	explicit Application(std::filesystem::path pref_path);
	~Application();
	int32_t run();

private:
	std::filesystem::path file_path;
	bool file_path_valid = false;
	std::filesystem::path cache_file_path;
	CGFileHandler cg_file_handler;
	TransactionModel transaction_model;
	AccountModel account_model;
	CategoryModel category_model;
	UI ui;

	bool load_file(const std::string& file_path, bool create);
	bool prompt_new_file();
	bool prompt_open_file();
};
