#pragma once

#include "inputs.hpp"

class TransactionModel;
class AccountModel;
class CategoryModel;

enum class DialogResult
{
	Accept,
	Cancel,
	None
};

class TransactionDialog
{
public:
	TransactionDialog() = default;
	void init(const TransactionModel& transaction_model, const CategoryModel& category_model, const Transaction& transaction = Transaction());
	DialogResult draw(const std::string& label, const TransactionModel& transaction_model);
	Transaction get_transaction();

private:
	DateInput date_input;
	CompletionInput description_input;
	AmountInput amount_input;
	Dropdown category_dropdown;
	Transaction transaction;
};

class TransactionGroupDialog
{
public:
	TransactionGroupDialog() = default;
	void init(const TransactionModel& transaction_model, const CategoryModel& category_model, const TransactionGroup& transaction_group = TransactionGroup());
	DialogResult draw(const std::string& label, const TransactionModel& transaction_model, const CategoryModel& category_model);
	TransactionGroup get_transaction_group();

private:
	TransactionDialog member_dialog;
	DateInput date_input;
	CompletionInput description_input;
	Dropdown category_dropdown;
	TransactionGroup transaction_group;
	int32_t selected_group_row = -1;

	void draw_transaction_table(const TransactionModel& transaction_model, const CategoryModel& category_model);
};

class AccountsDialog
{
public:
	void init(AccountModel& account_model, int64_t transaction_total_amount);
	void draw(AccountModel& account_model);

private:
	StringInput name_input;
	AmountInput amount_input;
	int64_t transaction_total_amount;
	int64_t account_total_amount;
	int32_t selected_row;
	int32_t opened_row;
};
