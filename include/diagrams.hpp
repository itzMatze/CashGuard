#pragma once

#include "category_model.hpp"
#include "transaction_model.hpp"
#include "transaction.hpp"
#include <chrono>

class Diagrams
{
public:
	enum class TimeRange
	{
		Day,
		Week,
		Month,
		Year,
		All,
		Custom
	};

	Diagrams()
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		m_custom_start = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(now)};
		m_custom_end = m_custom_start;
	}

	void draw_pie_chart(const CategoryModel& categories, const TransactionModel& transaction_model, ImVec2 size);
	void draw_bar_group(const CategoryModel& categories, const TransactionModel& transaction_model, ImVec2 size);
	void draw_spending_over_time(const CategoryModel& categories, const TransactionModel& transaction_model, ImVec2 size);

private:
	TimeRange m_current_range = TimeRange::All;

	std::chrono::year_month_day m_custom_start;
	std::chrono::year_month_day m_custom_end;

	void render_date_input(const char* id, std::chrono::year_month_day& date);
};