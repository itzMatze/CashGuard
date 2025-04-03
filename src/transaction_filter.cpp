#include "transaction_filter.hpp"
#include "transaction.hpp"

bool TransactionFilter::check(const Transaction& transaction) const
{
	if (!active) return true;
	if (transaction.date > dateMax || transaction.date < dateMin) return false;
	if (category != Category::None && transaction.category != category) return false;
	if (transaction.amount.value > amountMax.value || transaction.amount.value < amountMin.value) return false;
	if (searchPhrase.size() > 0 && !transaction.description.contains(searchPhrase, Qt::CaseInsensitive)) return false;
	return true;
}

