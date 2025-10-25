#pragma once

#include "transaction_model.hpp"
#include <QtCharts/QChart>
#include <qlineseries.h>

Amount get_filtered_total_amount(const TransactionModel& transaction_model);
Amount get_global_total_amount(const TransactionModel& transaction_model);
std::pair<QChart*, QLineSeries*> get_small_total_amount_chart(const TransactionModel& transaction_model);
std::pair<QChart*, QLineSeries*> get_total_amount_chart(const TransactionModel& transaction_model);
