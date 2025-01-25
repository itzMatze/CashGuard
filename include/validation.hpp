#pragma once

#include <cstdint>
#include "transaction_model.hpp"

bool validateTransactionIndex(uint32_t index, const TransactionModel& transaction_model, QWidget* parent = nullptr);
