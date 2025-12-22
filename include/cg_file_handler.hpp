#pragma once

#include <string>
#include "account_model.hpp"
#include "transaction_model.hpp"

class CGFileHandler
{
public:
	[[nodiscard]] bool save_to_file(const std::string& file_path, const TransactionModel& transaction_model, const AccountModel& account_model);
	[[nodiscard]] bool load_from_file(const std::string& file_path, TransactionModel& transaction_model, AccountModel& account_model);

private:
	std::size_t hash;
};
