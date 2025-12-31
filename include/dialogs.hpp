#pragma once

#include "account_model.hpp"
#include "inputs.hpp"
#include "transaction_model.hpp"

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
	void init(const TransactionModel& transaction_model, const Transaction& transaction = Transaction());
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
	void init(const TransactionModel& transaction_model, const TransactionGroup& transaction_group = TransactionGroup());
	DialogResult draw(const std::string& label, const TransactionModel& transaction_model);
	TransactionGroup get_transaction_group();

private:
	TransactionDialog member_dialog;
	DateInput date_input;
	CompletionInput description_input;
	Dropdown category_dropdown;
	TransactionGroup transaction_group;
	int32_t selected_group_row = 0;

	void draw_transaction_table(const TransactionModel& transaction_model);
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
