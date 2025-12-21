#include "account_model.hpp"
#include "mainwindow_ui.hpp"
#include "small_total_amount_chart.hpp"
#include "transaction_table_style_delegate.hpp"
#include "total_amount_chart.hpp"
#include "transaction_model.hpp"
#include "qt_util.hpp"
#include <QBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineSeries>
#include <QMainWindow>
#include <QPushButton>
#include <QTableView>
#include <QTabWidget>
#include <QWidget>

MainWindowUI::MainWindowUI(QWidget* parent)
{
	central_widget = new QWidget(parent);
	tab_widget = new QTabWidget(central_widget);
	tab_transaction_table = new QWidget(tab_widget);
	total_amount_label = new QLabel(tab_transaction_table);
	small_total_amount_chart_view = new SmallTotalAmountChartView(tab_transaction_table);
	table_view = new QTableView(tab_transaction_table);
	add_button = new QPushButton(tab_transaction_table);
	add_group_button = new QPushButton(tab_transaction_table);
	edit_button = new QPushButton(tab_transaction_table);
	remove_button = new QPushButton(tab_transaction_table);
	filter_button = new QPushButton(tab_transaction_table);
	account_button = new QPushButton(tab_transaction_table);
	tab_total_amount_graph = new QWidget(tab_widget);
	total_amount_chart_view = new TotalAmountChartView(tab_total_amount_graph);

	QVBoxLayout* root_layout = new QVBoxLayout(central_widget);
	central_widget->setLayout(root_layout);
	root_layout->addWidget(tab_widget);

	// tab 0
	QVBoxLayout* tab0_root_layout = new QVBoxLayout(tab_transaction_table);
	tab_widget->addTab(tab_transaction_table, "Transaction Table");
	tab_transaction_table->setLayout(tab0_root_layout);
	// total amount value and graph
	total_amount_label->setFont(set_font_size(20, true, total_amount_label->font()));
	total_amount_label->setIndent(20);
	QHBoxLayout* total_amount_layout = new QHBoxLayout;
	total_amount_layout->addWidget(total_amount_label);
	small_total_amount_chart_view->setRenderHint(QPainter::Antialiasing);
	total_amount_layout->addWidget(small_total_amount_chart_view);
	tab0_root_layout->addLayout(total_amount_layout);
	tab0_root_layout->setStretchFactor(total_amount_layout, 1);
	// transaction table
	table_view->setFont(set_font_size(12, false, table_view->font()));
	tab0_root_layout->addWidget(table_view);
	tab0_root_layout->setStretchFactor(table_view, 4);
	table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	table_view->setSelectionMode(QAbstractItemView::SingleSelection);
	table_view->setItemDelegate(new TransactionTableStyleDelegate(table_view));
	table_view->horizontalHeader()->setHighlightSections(false);
	table_view->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
	table_view->verticalHeader()->setHighlightSections(false);
	table_view->verticalHeader()->setFocusPolicy(Qt::NoFocus);
	// buttons
	add_button->setFont(set_font_size(12, false, add_button->font()));
	add_button->setText("Add");
	add_group_button->setFont(set_font_size(12, false, add_group_button->font()));
	add_group_button->setText("Add Group");
	edit_button->setFont(set_font_size(12, false, edit_button->font()));
	edit_button->setText("Edit");
	remove_button->setFont(set_font_size(12, false, remove_button->font()));
	remove_button->setText("Remove");
	filter_button->setFont(set_font_size(12, false, filter_button->font()));
	filter_button->setText("Filter");
	account_button->setFont(set_font_size(12, false, account_button->font()));
	account_button->setText("Accounts");
	QHBoxLayout* button_layout = new QHBoxLayout;
	button_layout->addWidget(add_button);
	button_layout->addWidget(add_group_button);
	button_layout->addWidget(edit_button);
	button_layout->addWidget(remove_button);
	button_layout->addWidget(filter_button);
	button_layout->addWidget(account_button);
	tab0_root_layout->addLayout(button_layout);

	// tab 1
	QVBoxLayout* tab1_root_layout = new QVBoxLayout(tab_total_amount_graph);
	tab_widget->addTab(tab_total_amount_graph, "Total Amount Graph");
	tab_total_amount_graph->setLayout(tab1_root_layout);
	total_amount_chart_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	total_amount_chart_view->setRenderHint(QPainter::Antialiasing);
	tab1_root_layout->addWidget(total_amount_chart_view);
}

void MainWindowUI::update(const TransactionModel& transaction_model, const AccountModel& account_model, const Amount& filtered_total_amount, const Amount& global_total_amount)
{
	total_amount_label->setText(filtered_total_amount.to_string() + " €");
	small_total_amount_chart_view->update(transaction_model, transaction_model.get_filter().date_min);
	total_amount_chart_view->update(transaction_model, transaction_model.get_filter().date_min);
	if (account_model.get_total_amount().value == global_total_amount.value)
	{
		account_button->setStyleSheet("QPushButton { color: #00ff00; }");
	}
	else
{
		account_button->setStyleSheet("QPushButton { color: #ff0000; }");
	}
}
