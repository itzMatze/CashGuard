#pragma once

#include "dialogs.hpp"
#include "graphs.hpp"
#include "transaction_filter.hpp"
#include "transaction_table.hpp"

class TransactionModel;
class AccountModel;
class CategoryModel;

class UI
{
public:
	UI() = default;
	void init(const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model);
	void draw(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model, CategoryModel& category_model);

private:
	TransactionModel filtered_transaction_model;
	TransactionFilter transaction_filter;
	TotalAmountGraph total_amount_graph;
	TransactionTable transaction_table;
	TransactionDialog transaction_dialog;
	TransactionGroupDialog transaction_group_dialog;
	AccountsDialog accounts_dialog;
	bool show_amounts = false;

	void draw_transaction_tab(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model, CategoryModel& category_model);
	void draw_graph_tab(ImVec2 available_space, const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model);
};
