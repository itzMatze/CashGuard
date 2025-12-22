#include "transaction_filter.hpp"
#include "transaction.hpp"

bool contains_substring_case_insensitive(const std::string& text, const std::string& substring)
{
	auto it = std::search(
		text.begin(), text.end(),
		substring.begin(), substring.end(),
		[](char a, char b) {
			return std::tolower((unsigned char)a)
			== std::tolower((unsigned char)b);
		}
	);
	return it != text.end();
}

bool TransactionFilter::check(const Transaction& transaction) const
{
	if (!active) return true;
	// if any is invalid return false if filter is not negated and true if it is negated
	if (transaction.date > date_max || transaction.date < date_min) return negated;
	if (category.size() > 0 && category != "None" && transaction.category != category) return negated;
	if (transaction.amount.value > amount_max.value || transaction.amount.value < amount_min.value) return negated;
	if (search_phrase.size() > 0 && !contains_substring_case_insensitive(transaction.description, search_phrase)) return negated;
	// filter is valid
	return !negated;
}

