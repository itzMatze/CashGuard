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
	Application() = default;
	bool load_file(const std::string& file_path);
	int32_t run();

private:
	std::filesystem::path file_path;
	bool file_path_valid = false;
	CGFileHandler cg_file_handler;
	TransactionModel transaction_model;
	AccountModel account_model;
	CategoryModel category_model;
	UI ui;

	bool prompt_new_file();
	bool prompt_open_file();
};
