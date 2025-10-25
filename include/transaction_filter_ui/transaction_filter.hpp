#pragma once

#include "transaction.hpp"
#include <limits>

class TransactionFilter
{
public:
	TransactionFilter() = default;
	bool check(const Transaction& transaction) const;

	QDate date_min = QDate::currentDate().addYears(-80);
	QDate date_max = QDate::currentDate().addMonths(1);
	QString category = "None";
	Amount amount_min = std::numeric_limits<int32_t>::min();
	Amount amount_max = std::numeric_limits<int32_t>::max();
	QString search_phrase = "";
	bool active = false;
	bool negated = false;
};
