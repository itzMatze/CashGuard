#pragma once

#include "imgui.h"
#include "transaction_model.hpp"

class TotalAmountGraph
{
public:
	TotalAmountGraph() = default;
	void update_data(const TransactionModel& transaction_model);
	void draw_small_graph(ImVec2 available_space);
	void draw_large_graph(ImVec2 available_space);

private:
	std::vector<double> time_points;
	std::vector<double> data_points;
	double min_amount;
	double max_amount;
	int64_t unix_seconds_now;
};
