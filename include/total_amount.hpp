#pragma once

#include "transaction_model.hpp"
#include <QtCharts/QChart>

Amount getCurrentTotalAmount(const TransactionModel& transactionModel);
QChart* getSmallTotalAmountChart(const TransactionModel& transactionModel);
