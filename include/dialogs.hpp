#pragma once

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
	DialogResult draw(const std::string& label);
	Transaction get_transaction() const;

private:
	DateInput date_input;
	StringInput description_input;
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
	TransactionGroup get_transaction_group() const;

private:
	TransactionDialog member_dialog;
	DateInput date_input;
	StringInput description_input;
	Dropdown category_dropdown;
	TransactionGroup transaction_group;
	int32_t selected_group_row = 0;

	void draw_transaction_table(const TransactionModel& transaction_model);
};
