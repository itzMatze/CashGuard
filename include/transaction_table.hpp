#pragma once

#include "imgui.h"
#include <cstdint>
#include <memory>

class Transaction;
class TransactionModel;
class CategoryModel;

class TransactionTable
{
public:
	TransactionTable() = default;
	void draw(ImVec2 available_space, const TransactionModel& transaction_model, const CategoryModel& category_model, bool show_amounts);
	int32_t get_selected_row() const;
	std::shared_ptr<const Transaction> get_selected_transaction() const;

private:
	int32_t selected_row = -1;
	std::shared_ptr<const Transaction> selected_transaction = nullptr;
};
