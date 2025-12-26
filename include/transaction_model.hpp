#pragma once

#include "transaction_filter.hpp"
#include "util/color.hpp"
#include <unordered_map>
#include <vector>

class TransactionModel
{
public:
	TransactionModel() = default;
	int32_t count() const;
	const std::shared_ptr<Transaction> at(int32_t index) const;
	void add(const std::shared_ptr<Transaction> transaction);
	void remove(int32_t index);
	void set(int32_t index, const std::shared_ptr<Transaction> transaction);
	const std::vector<std::shared_ptr<Transaction>>& get_unfiltered_transactions() const;
	void clear();
	void set_filter_active(bool active);
	void set_filter(const TransactionFilter& new_filter);
	const TransactionFilter& get_filter() const;
	// use with caution, changes to the filter will not trigger a filter update
	TransactionFilter& get_filter();
	bool is_empty() const;
	std::vector<std::string> get_unique_value_list(const std::string& field_name) const;
	bool get_auto_complete_transaction(const std::string& description, std::shared_ptr<const Transaction>& completed_transaction) const;
	void add_category(const std::string& name, const Color& color);
	const std::vector<std::string>& get_category_names() const;
	const std::unordered_map<std::string, Color>& get_category_colors() const;
	void set_categories(const std::vector<std::string>& category_names, const std::unordered_map<std::string, Color>& category_colors);
	Amount get_filtered_total_amount() const;
	Amount get_global_total_amount() const;

private:
	std::vector<std::string> category_names;
	std::unordered_map<std::string, Color> category_colors;
	std::vector<std::shared_ptr<Transaction>> transactions;
	std::vector<std::shared_ptr<Transaction>> filtered_transactions;
	TransactionFilter filter;

	int32_t get_transaction_index(std::shared_ptr<Transaction> transaction);
	void reset();
};
