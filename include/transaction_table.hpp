#pragma once

#include "imgui.h"
#include <cstdint>
class TransactionModel;

class TransactionTable
{
public:
	TransactionTable() = default;
	void draw(ImVec2 available_space, const TransactionModel& transaction_model);
	int32_t get_selected_row() const;

private:
	int32_t selected_row = -1;
};
