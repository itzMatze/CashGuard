#include "total_amount.hpp"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>

Amount get_filtered_total_amount(const TransactionModel& transaction_model)
{
	int32_t total_amount = 0;
	for (uint32_t i = 0; i < transaction_model.rowCount(); i++) total_amount += transaction_model.get_transaction(i)->amount.value;
	return Amount(total_amount);
}

Amount get_global_total_amount(const TransactionModel& transaction_model)
{
	int32_t total_amount = 0;
	for (std::shared_ptr<const Transaction> t : transaction_model.get_unfiltered_transactions()) total_amount += t->amount.value;
	return Amount(total_amount);
}
