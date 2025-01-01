#pragma once

#include <QString>
#include "transaction_model.hpp"

[[nodiscard]] bool saveToFile(const QString& filePath, const TransactionModel& transactionModel);
[[nodiscard]] bool loadFromFile(const QString& filePath, TransactionModel& transactionModel);
