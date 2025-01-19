#pragma once

#include "transaction_model.hpp"
#include <QtCharts/QChart>
#include <qlineseries.h>

Amount getCurrentTotalAmount(const TransactionModel& transactionModel);
std::pair<QChart*, QLineSeries*> getSmallTotalAmountChart(const TransactionModel& transactionModel);
