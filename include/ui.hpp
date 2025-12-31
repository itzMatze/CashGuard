#pragma once

#include "dialogs.hpp"
#include "graphs.hpp"
#include "transaction_table.hpp"

class TransactionModel;
class AccountModel;

class UI
{
public:
	UI() = default;
	void init(TransactionModel& transaction_model, AccountModel& account_model);
	void draw(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model);

private:
	TotalAmountGraph total_amount_graph;
	TransactionTable transaction_table;
	TransactionDialog transaction_dialog;
	TransactionGroupDialog transaction_group_dialog;
	AccountsDialog accounts_dialog;

	void draw_transaction_tab(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model);
	void draw_graph_tab(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model);
};
