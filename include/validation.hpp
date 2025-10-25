#pragma once

#include <cstdint>
#include "transaction_model.hpp"

bool validate_transaction_index(uint32_t index, const TransactionModel& transaction_model);
