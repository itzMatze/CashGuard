#include "transaction_model.hpp"
#include "transaction.hpp"

#include <memory>
#include <set>

int32_t TransactionModel::count() const
{
	return transactions.size();
}

const std::shared_ptr<const Transaction> TransactionModel::at(int32_t index) const
{
	return transactions[index];
}

void TransactionModel::add(const std::shared_ptr<const Transaction> transaction)
{
	int32_t index = 0;
	while (index < transactions.size() && *transaction < *transactions[index]) index++;
	transactions.insert(transactions.begin() + index, transaction);
	dirty = true;
}

void TransactionModel::remove(int32_t index)
{
	transactions.erase(transactions.begin() + index);
	dirty = true;
}

int32_t binary_search_index(std::vector<std::shared_ptr<const Transaction>>& transactions, const std::shared_ptr<const Transaction> value)
{
	int32_t l = 0;
	int32_t r = transactions.size() - 1;

	while (l < r)
	{
		int32_t m = (l + r) / 2;
		if (*transactions[m] > *(value)) l = m + 1;
		else r = m;
	}
	return *transactions[l] == *value ? l : -1;
}

void TransactionModel::remove(const std::shared_ptr<const Transaction> transaction)
{
	int32_t index = binary_search_index(transactions, transaction);
	if (index == -1) return;
	remove(index);
	dirty = true;
}

void TransactionModel::set(int32_t index, const std::shared_ptr<const Transaction> transaction)
{
	remove(index);
	add(transaction);
}

void TransactionModel::set(const std::shared_ptr<const Transaction> old_transaction, const std::shared_ptr<const Transaction> new_transaction)
{
	remove(old_transaction);
	add(new_transaction);
}

const std::vector<std::shared_ptr<const Transaction>>& TransactionModel::get_transactions() const
{
	return transactions;
}

void TransactionModel::clear()
{
	transactions.clear();
	dirty = true;
}

bool TransactionModel::is_empty() const
{
	return transactions.size() == 0;
}

std::vector<std::string> TransactionModel::get_unique_value_list(int32_t field_index) const
{
	std::set<std::string> unique_values;
	for (const std::shared_ptr<const Transaction> transaction : transactions) unique_values.emplace(transaction->get_field(field_index));
	std::vector<std::string> values;
	for (const std::string& value : unique_values) values.push_back(value);
	return values;
}

bool TransactionModel::get_auto_complete_transaction(const std::string& description, std::shared_ptr<const Transaction>& completedTransaction) const
{
	completedTransaction = nullptr;
	for (const std::shared_ptr<const Transaction> transaction : transactions)
	{
		if (transaction->get_field(TRANSACTION_FIELD_DESCRIPTION) == description)
		{
			completedTransaction = transaction;
			return true;
		}
	}
	return false;
}

Amount TransactionModel::get_total_amount() const
{
	int32_t total_amount = 0;
	for (const std::shared_ptr<const Transaction> t : transactions) total_amount += t->amount.value;
	return Amount(total_amount);
}

int32_t TransactionModel::get_transaction_index(std::shared_ptr<const Transaction> transaction) const
{
	for (uint32_t i = 0; i < transactions.size(); i++) if (transactions.at(i) == transaction) return i;
	return transactions.size();
}
