#pragma once

#include "imgui.h"
#include "transaction_model.hpp"

class SmallTotalAmountGraph
{
public:
	SmallTotalAmountGraph() = default;
	void update_data(const TransactionModel& transaction_model);
	void draw(ImVec2 available_space);

private:
	std::vector<double> time_points;
	std::vector<double> data_points;
	std::vector<double> month_time_points;
	std::vector<std::string> month_labels;
	double min_amount;
	double max_amount;
	int64_t unix_seconds_now;
};
