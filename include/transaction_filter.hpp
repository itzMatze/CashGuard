#pragma once

#include "dialogs.hpp"
#include "transaction.hpp"
#include "util/color.hpp"
#include <string>
#include <vector>

class TransactionFilter
{
public:
	TransactionFilter();
	void init(const CategoryModel& category_model);
	void reset();
	DialogResult draw(const std::string& label);
	bool check(const std::shared_ptr<const Transaction> transaction) const;

private:
	// Description
	struct StringEditTable
	{
		std::vector<std::string> strings;
		StringInput input;
		int32_t selected_row;
		int32_t opened_row;
		bool set_focus;

		void init();
		void draw(const std::string& label);
		void obtain_results();
	};
	StringEditTable search_phrases_table;
	StringEditTable ignore_phrases_table;

	// Category
	void draw_category_table(const std::string& label);
	struct CategoryEntry
	{
		std::string name;
		Color color;
		bool selected;
	};
	std::vector<CategoryEntry> category_entries;

	// Date
	struct DateRangeTable
	{
		struct DateRange
		{
			Date begin;
			Date end;
		};
		std::vector<DateRange> date_ranges;
		DateInput begin_input;
		DateInput end_input;
		bool active;
		int32_t selected_row;
		int32_t opened_row;
		bool set_focus;

		void init();
		void draw(const std::string& label);
		void obtain_results();
	} date_range_table;

	// Amount
	struct AmountRangeTable
	{
		struct AmountRange
		{
			Amount lower;
			Amount upper;
		};
		std::vector<AmountRange> amount_ranges;
		AmountInput lower_input;
		AmountInput upper_input;
		bool active;
		int32_t selected_row;
		int32_t opened_row;
		bool set_focus;

		void init();
		void draw(const std::string& label);
		void obtain_results();
	} amount_range_table;
};
