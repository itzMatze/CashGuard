#pragma once

#include "transaction.hpp"
#include <limits>

class TransactionFilter
{
public:
	TransactionFilter() = default;
	bool check(const Transaction& transaction) const;

	Date date_min = to_date(Clock::now()) - std::chrono::years(80);
	Date date_max = to_date(Clock::now()) + std::chrono::years(1);
	std::string category = "";
	Amount amount_min = std::numeric_limits<int64_t>::min();
	Amount amount_max = std::numeric_limits<int64_t>::max();
	std::string search_phrase = "";
	bool active = false;
	bool negated = false;
};
