#pragma once
#include "cg_file_handler.hpp"
#include "transaction_model.hpp"
#include "transaction_table.hpp"
#include <string>

class Application
{
public:
	Application() = default;
	bool init(const std::string& file_path);
	int32_t run();

private:
	std::string file_path;
	CGFileHandler cg_file_handler;
	TransactionModel transaction_model;
	AccountModel account_model;
	TransactionTable transaction_table;
};
