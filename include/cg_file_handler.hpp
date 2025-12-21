#pragma once

#include <QString>
#include "account_model.hpp"
#include "transaction_model.hpp"

class CGFileHandler
{
public:
	[[nodiscard]] bool save_to_file(const QString& file_path, const TransactionModel& transaction_model, const AccountModel& account_model);
	[[nodiscard]] bool load_from_file(const QString& file_path, TransactionModel& transaction_model, AccountModel& account_model);

private:
	uint32_t hash;
};
