#include "transaction_filter_ui/transaction_filter.hpp"
#include "transaction.hpp"

bool TransactionFilter::check(const Transaction& transaction) const
{
	if (!active) return true;
	// if any is invalid return false if filter is not negated and true if it is negated
	if (transaction.date > date_max || transaction.date < date_min) return negated;
	if (category.size() > 0 && category != "None" && transaction.category != category) return negated;
	if (transaction.amount.value > amount_max.value || transaction.amount.value < amount_min.value) return negated;
	if (search_phrase.size() > 0 && !transaction.description.contains(search_phrase, Qt::CaseInsensitive)) return negated;
	// filter is valid
	return !negated;
}

