#pragma once

#include "transaction_model.hpp"

Amount get_filtered_total_amount(const TransactionModel& transaction_model);
Amount get_global_total_amount(const TransactionModel& transaction_model);
