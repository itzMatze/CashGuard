#include "transaction_filter_ui/transaction_filter.hpp"
#include "transaction.hpp"

bool TransactionFilter::check(const Transaction& transaction) const
{
	if (!active) return true;
	// if any is invalid return false if filter is not negated and true if it is negated
	if (transaction.date > dateMax || transaction.date < dateMin) return negated;
	if (category.size() > 0 && category != "None" && transaction.category != category) return negated;
	if (transaction.amount.value > amountMax.value || transaction.amount.value < amountMin.value) return negated;
	if (searchPhrase.size() > 0 && !transaction.description.contains(searchPhrase, Qt::CaseInsensitive)) return negated;
	// filter is valid
	return !negated;
}

