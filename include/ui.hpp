#pragma once

#include "dialogs.hpp"
#include "transaction_filter.hpp"
#include "transaction_table.hpp"
#include "visualizations.hpp"

class TransactionModel;
class AccountModel;
class CategoryModel;
class Diagrams;

class UI
{
public:
	UI() = default;
	void init(const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model);
	void draw(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model, CategoryModel& category_model, bool valid_file);
	bool open_existing_file;
	bool create_new_file;

private:

	enum class Chart : int32_t
	{
		BarChart = 0,
		PieChart = 1,
		Count = 2
	};

	static constexpr std::array<const char*, 2> diagram_chart_names = { "Bar Chart", "Pie Chart"};

	TransactionModel filtered_transaction_model;
	TransactionFilter transaction_filter;
	TotalAmountGraph total_amount_graph;
	TransactionTable transaction_table;
	TransactionDialog transaction_dialog;
	AccountsDialog accounts_dialog;
	CategoriesDialog categories_dialog;
	Diagrams diagrams;
	Chart current_selection = Chart::BarChart;
	bool show_amounts = false;

	void draw_transaction_tab(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model, CategoryModel& category_model, bool valid_file);
	void draw_graph_tab(ImVec2 available_space, const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model);
	void draw_bar_tab(ImVec2 available_space, const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model);
	void draw_diagram_tab(ImVec2 available_space, const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model);
	void draw_filter(const TransactionModel& transaction_model, const CategoryModel& category_model, TransactionModel& filtered_transaction_model, TransactionFilter& transaction_filter, TotalAmountGraph& total_amount_graph);
};
