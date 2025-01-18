#pragma once

#include "transaction.hpp"
#include <limits>

class TransactionFilter
{
public:
	TransactionFilter() = default;
	bool check(const Transaction& transaction) const;

	QDate dateMin = QDate(1900, 1, 1);
	QDate dateMax = QDate(3000, 1, 1);
	QString category = transactionCategories.back();
	Amount amountMin = std::numeric_limits<int32_t>::min();
	Amount amountMax = std::numeric_limits<int32_t>::max();
	QString searchPhrase = "";
};
