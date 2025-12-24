#pragma once

#include "imgui.h"
#include "transaction.hpp"
#include "transaction_table.hpp"

class TransactionModel;
class AccountModel;

class TransactionPage
{
public:
	TransactionPage() = default;
	void draw(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model);

private:
	TransactionTable transaction_table;

	Transaction opened_transaction;
	TransactionGroup opened_transaction_group;
	int32_t selected_group_row = 0;
	void transaction_dialog(TransactionModel& transaction_model, int32_t transaction_index = -1);
	void transaction_member_dialog(const std::vector<std::string>& category_names, int32_t transaction_index = -1);
	void transaction_group_dialog(TransactionModel& transaction_model, int32_t transaction_index = -1);
};
