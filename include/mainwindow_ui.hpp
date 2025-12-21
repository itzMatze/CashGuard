#pragma once

class AccountModel;
struct Amount;
class SmallTotalAmountChartView;
class TransactionModel;
class TotalAmountChartView;
class QLabel;
class QTableView;
class QTabWidget;
class QPushButton;
class QWidget;

class MainWindowUI
{
public:
	explicit MainWindowUI(QWidget* parent);
	void update(const TransactionModel& transaction_model, const AccountModel& account_model, const Amount& filtered_total_amount, const Amount& global_total_amount);

	QWidget* central_widget;

	QTabWidget* tab_widget;
	QWidget* tab_transaction_table;
	QLabel* total_amount_label;
	SmallTotalAmountChartView* small_total_amount_chart_view;
	QTableView* table_view;
	QPushButton* add_button;
	QPushButton* add_group_button;
	QPushButton* edit_button;
	QPushButton* remove_button;
	QPushButton* filter_button;
	QPushButton* account_button;

	QWidget* tab_total_amount_graph;
	TotalAmountChartView* total_amount_chart_view;
};
