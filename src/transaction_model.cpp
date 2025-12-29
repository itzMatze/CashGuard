#include "transaction_model.hpp"
#include "transaction.hpp"

#include <memory>
#include <set>

int32_t TransactionModel::count() const
{
	return filtered_transactions.size();
}

const std::shared_ptr<const Transaction> TransactionModel::at(int32_t index) const
{
	return filtered_transactions[index];
}

void TransactionModel::add(const std::shared_ptr<const Transaction> transaction)
{
	int32_t index = 0;
	while (index < transactions.size() && *transaction < *transactions[index]) index++;
	transactions.insert(transactions.begin() + index, transaction);
	if (!filter.check(*transaction)) return;
	index = 0;
	while (index < filtered_transactions.size() && *transaction < *filtered_transactions[index]) index++;
	filtered_transactions.insert(filtered_transactions.begin() + index, transaction);
}

void TransactionModel::remove(int32_t index)
{
	transactions.erase(transactions.begin() + get_transaction_index(filtered_transactions.at(index)));
	filtered_transactions.erase(filtered_transactions.begin() + index);
}

void TransactionModel::set(int32_t index, const std::shared_ptr<const Transaction> transaction)
{
	remove(index);
	add(transaction);
}

const std::vector<std::shared_ptr<const Transaction>>& TransactionModel::get_unfiltered_transactions() const
{
	return transactions;
}

void TransactionModel::clear()
{
	transactions.clear();
	filtered_transactions.clear();
}

void TransactionModel::set_filter_active(bool active)
{
	filter.active = active;
	reset();
}

void TransactionModel::set_filter(const TransactionFilter& filter)
{
	this->filter = filter;
	reset();
}

const TransactionFilter& TransactionModel::get_filter() const
{
	return filter;
}

TransactionFilter& TransactionModel::get_filter()
{
	return filter;
}

bool TransactionModel::is_empty() const
{
	return transactions.size() == 0;
}

std::vector<std::string> TransactionModel::get_unique_value_list(const std::string& field_name) const
{
	std::set<std::string> unique_values;
	for (const std::shared_ptr<const Transaction> transaction : transactions) unique_values.emplace(transaction->get_field(field_name));
	std::vector<std::string> values;
	for (const std::string& value : unique_values) values.push_back(value);
	return values;
}

bool TransactionModel::get_auto_complete_transaction(const std::string& description, std::shared_ptr<const Transaction>& completedTransaction) const
{
	completedTransaction = nullptr;
	for (const std::shared_ptr<const Transaction> transaction : transactions)
	{
		if (transaction->get_field(TransactionFieldNames::Description) == description)
		{
			completedTransaction = transaction;
			return true;
		}
	}
	return false;
}

void TransactionModel::add_category(const std::string& name, const Color& color)
{
	category_names.push_back(name);
	category_colors.emplace(name, color);
}

const std::vector<std::string>& TransactionModel::get_category_names() const
{
	return category_names;
}

const std::unordered_map<std::string, Color>& TransactionModel::get_category_colors() const
{
	return category_colors;
}

Amount TransactionModel::get_filtered_total_amount() const
{
	int32_t total_amount = 0;
	for (uint32_t i = 0; i < filtered_transactions.size(); i++) total_amount += filtered_transactions[i]->amount.value;
	return Amount(total_amount);
}

Amount TransactionModel::get_global_total_amount() const
{
	int32_t total_amount = 0;
	for (const std::shared_ptr<const Transaction> t : transactions) total_amount += t->amount.value;
	return Amount(total_amount);
}

int32_t TransactionModel::get_transaction_index(std::shared_ptr<const Transaction> transaction)
{
	for (uint32_t i = 0; i < transactions.size(); i++) if (transactions.at(i) == transaction) return i;
	return transactions.size();
}

void TransactionModel::reset()
{
	std::vector<std::shared_ptr<const Transaction>> transactions_copy(transactions);
	clear();
	for (std::shared_ptr<const Transaction> transaction : transactions_copy)
	{
		add(transaction);
	}
}
