#pragma once

#include <filesystem>
#include "account_model.hpp"
#include "category_model.hpp"
#include "transaction_model.hpp"

class CGFileHandler
{
public:
	[[nodiscard]] bool save_to_file(const std::filesystem::path& file_path, const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model);
	[[nodiscard]] bool load_from_file(const std::filesystem::path& file_path, TransactionModel& transaction_model, AccountModel& account_model, CategoryModel& category_model);

private:
	std::size_t hash;
};
