#pragma once

#include "imgui.h"
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
};
