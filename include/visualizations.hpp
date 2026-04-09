#pragma once

#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include "category_model.hpp"
#include "transaction_model.hpp"

#include <map>

class TotalAmountGraph
{
public:
	TotalAmountGraph() = default;
	void update_data(const TransactionModel& transaction_model);
	void update_data(const CategoryModel& categories, const TransactionModel& transaction_model);
	void draw_small_graph(ImVec2 available_space, bool show_amounts);
	void draw_large_graph(ImVec2 available_space);
	void draw_bar_spending_graph(const TransactionModel& transaction_model, ImVec2 available_space);
	
private:
	struct BarChartData
	{
		std::vector<double> xs;
		std::vector<double> incomes;
		std::vector<double> expenses;
		std::vector<double> net;
		double income_peak = 0.0;
    double expense_peak = 0.0;
	};
	
	struct BarTotals
	{
		double temp_incomes = 0.0;
		double temp_expenses = 0.0;
	};
	
	enum class Timeframe : int32_t
	{
		Day = 0,
		Week = 1,
		Month = 2,
		Year = 3,
		Count = 4
	};

	static constexpr std::array<const char*, 4> timeframe_names = { "Day", "Week", "Month", "Year" };
	
	std::vector<double> time_points;
	std::vector<double> data_points;
	std::map<int, BarChartData> bucket_data;
	Timeframe current_selection = Timeframe::Month;
	double min_amount;
	double max_amount;
	int64_t unix_seconds_now;
};

class Diagrams
{
	public:
		Diagrams();
		void update_data(const CategoryModel& categories, const TransactionModel& transaction_model);
		void draw_pie_chart(ImVec2 size);
		void draw_bar_group(ImVec2 size);

	private:
		struct CategoryData
		{
			std::vector<double> values;
			std::vector<double> incomes;
    	std::vector<double> expenses;
			std::vector<double> net;
			std::vector<ImU32> colors;
			std::vector<std::string> names; 
			std::vector<std::string> labels;
			std::vector<const char*> names_ptr;
			std::vector<const char*> labels_ptr;
			std::vector<double> positions;
		};

		enum class TimeRange : int32_t
		{
			Day = 0, 
			Week = 1, 
			Month = 2, 
			Year = 3, 
			All = 4, 
			Custom = 5,
			Count = 6
		};
		
		static constexpr std::array<const char*, 6> range_names = { "Day", "Week", "Month", "Year", "All", "Custom"};
		std::map<TimeRange, CategoryData> precalculated_data;

		TimeRange current_range = TimeRange::All;
		Date custom_start;
		Date custom_end;
		int32_t last_data_hash = 0;
		ImPlotColormap dynamic_cmap = -1;
};

