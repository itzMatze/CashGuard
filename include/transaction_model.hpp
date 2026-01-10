#pragma once

#include "transaction.hpp"
#include <unordered_map>
#include <vector>

class TransactionModel
{
public:
	TransactionModel() = default;
	int32_t count() const;
	const std::shared_ptr<const Transaction> at(int32_t index) const;
	void add(const std::shared_ptr<const Transaction> transaction);
	void remove(int32_t index);
	void remove(const std::shared_ptr<const Transaction> transaction);
	void set(int32_t index, const std::shared_ptr<const Transaction> transaction);
	void set(const std::shared_ptr<const Transaction> old_transaction, const std::shared_ptr<const Transaction> new_transaction);
	const std::vector<std::shared_ptr<const Transaction>>& get_transactions() const;
	void clear();
	bool is_empty() const;
	std::vector<std::string> get_unique_value_list(int32_t field_index) const;
	bool get_auto_complete_transaction(const std::string& description, std::shared_ptr<const Transaction>& completed_transaction) const;
	Amount get_total_amount() const;
	const std::unordered_map<uint64_t, int32_t>& get_category_usage_counts() const;

	mutable bool dirty = false;

private:
	std::vector<std::shared_ptr<const Transaction>> transactions;
	std::unordered_map<uint64_t, int32_t> category_usage_counts;

	int32_t get_transaction_index(std::shared_ptr<const Transaction> transaction) const;
};
