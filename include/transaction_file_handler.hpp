#pragma once

#include <QString>
#include "account_model.hpp"
#include "transaction_model.hpp"

[[nodiscard]] bool saveToFile(const QString& filePath, const TransactionModel& transactionModel, const AccountModel& accountModel);
[[nodiscard]] bool loadFromFile(const QString& filePath, TransactionModel& transactionModel, AccountModel& accountModel);
