#pragma once

#include "transaction.hpp"
#include <limits>

class TransactionFilter
{
public:
	TransactionFilter() = default;
	bool check(const Transaction& transaction) const;

	QDate dateMin = QDate::currentDate().addYears(-80);
	QDate dateMax = QDate::currentDate().addMonths(1);
	Category category = Category::None;
	Amount amountMin = std::numeric_limits<int32_t>::min();
	Amount amountMax = std::numeric_limits<int32_t>::max();
	QString searchPhrase = "";
	bool active = false;
};
