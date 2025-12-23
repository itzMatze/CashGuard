#pragma once

#include "imgui.h"
class TransactionModel;

class TransactionTable
{
public:
	TransactionTable() = default;
	void draw(ImVec2 available_space, const TransactionModel& transaction_model);

private:
};
