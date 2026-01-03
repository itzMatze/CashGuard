#pragma once

#include "dialogs.hpp"
#include "transaction.hpp"
#include <string>
#include <vector>

class TransactionFilter
{
public:
	TransactionFilter();
	DialogResult draw(const std::string& label);
	bool check(const std::shared_ptr<const Transaction> transaction) const;

private:
	struct StringEditTable
	{
		std::vector<std::string> strings;
		StringInput input;
		int32_t selected_row = -1;
		int32_t opened_row = -1;
		bool set_focus = false;

		void draw(const std::string& label);
		void obtain_results();
	};

	StringEditTable search_phrases_table;
	StringEditTable ignore_phrases_table;
};
