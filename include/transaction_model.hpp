#pragma once

#include "transaction.hpp"
#include "util/color.hpp"
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
	void set(int32_t index, const std::shared_ptr<const Transaction> transaction);
	const std::vector<std::shared_ptr<const Transaction>>& get_transactions() const;
	void clear();
	bool is_empty() const;
	std::vector<std::string> get_unique_value_list(const std::string& field_name) const;
	bool get_auto_complete_transaction(const std::string& description, std::shared_ptr<const Transaction>& completed_transaction) const;
	void add_category(const std::string& name, const Color& color);
	const std::vector<std::string>& get_category_names() const;
	const std::unordered_map<std::string, Color>& get_category_colors() const;
	Amount get_total_amount() const;

	mutable bool dirty = false;

private:
	std::vector<std::string> category_names;
	std::unordered_map<std::string, Color> category_colors;
	std::vector<std::shared_ptr<const Transaction>> transactions;

	int32_t get_transaction_index(std::shared_ptr<const Transaction> transaction) const;
};
