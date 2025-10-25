#pragma once

#include "account_model.hpp"
#include "table_style_delegate.hpp"
#include "tooltip_chart_view.hpp"
#include "total_amount.hpp"
#include "transaction_model.hpp"
#include "qt_util.hpp"
#include <qboxlayout.h>
#include <qchartview.h>
#include <qlabel.h>
#include <qlineseries.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <qtableview.h>
#include <qtabwidget.h>
#include <qwidget.h>

class MainWindowUI
{
public:
	explicit MainWindowUI(QWidget* parent)
		: central_widget(new QWidget(parent))
		, root_layout(new QVBoxLayout(central_widget))
		, tab_widget(new QTabWidget(central_widget))
		, tab_transaction_table(new QWidget(tab_widget))
		, tab0_root_layout(new QVBoxLayout(tab_transaction_table))
		, total_amount_label(new QLabel(tab_transaction_table))
		, total_amount_small_chart(new QChartView(tab_transaction_table))
		, table_view(new QTableView(tab_transaction_table))
		, add_button(new QPushButton(tab_transaction_table))
		, add_group_button(new QPushButton(tab_transaction_table))
		, edit_button(new QPushButton(tab_transaction_table))
		, remove_button(new QPushButton(tab_transaction_table))
		, filter_button(new QPushButton(tab_transaction_table))
		, account_button(new QPushButton(tab_transaction_table))
		, tab_total_amount_graph(new QWidget(tab_widget))
		, tab1_root_layout(new QVBoxLayout(tab_total_amount_graph))
		, total_amount_chart(new TooltipChartView(tab_total_amount_graph))
	{
		central_widget->setLayout(root_layout);
		root_layout->addWidget(tab_widget);

		// tab 0
		tab_widget->addTab(tab_transaction_table, "Transaction Table");
		tab_transaction_table->setLayout(tab0_root_layout);
		// total amount value and graph
		total_amount_label->setFont(set_font_size(20, true, total_amount_label->font()));
		total_amount_label->setIndent(20);
		QHBoxLayout* total_amount_layout = new QHBoxLayout;
		total_amount_layout->addWidget(total_amount_label);
		total_amount_small_chart->setRenderHint(QPainter::Antialiasing);
		total_amount_layout->addWidget(total_amount_small_chart);
		tab0_root_layout->addLayout(total_amount_layout);
		tab0_root_layout->setStretchFactor(total_amount_layout, 1);
		// transaction table
		table_view->setFont(set_font_size(12, false, table_view->font()));
		tab0_root_layout->addWidget(table_view);
		tab0_root_layout->setStretchFactor(table_view, 4);
		table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
		table_view->setSelectionMode(QAbstractItemView::SingleSelection);
		table_view->setItemDelegate(new TableStyleDelegate(table_view));
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
		tab_widget->addTab(tab_total_amount_graph, "Total Amount Graph");
		tab_total_amount_graph->setLayout(tab1_root_layout);
		total_amount_chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		total_amount_chart->setRenderHint(QPainter::Antialiasing);
		tab1_root_layout->addWidget(total_amount_chart);
	}

	void update(const TransactionModel& transaction_model, const AccountModel& account_model, const Amount& filtered_total_amount, const Amount& global_total_amount)
	{
		total_amount_label->setText(filtered_total_amount.to_string() + " €");
		total_amount_small_chart->setChart(get_small_total_amount_chart(transaction_model).first);
		std::pair<QChart*, QLineSeries*> total_amount_data = get_total_amount_chart(transaction_model);
		total_amount_chart->update(total_amount_data.first, total_amount_data.second, transaction_model.get_filter().date_min);
		if (account_model.get_total_amount().value == global_total_amount.value)
		{
			account_button->setStyleSheet("QPushButton { color: #00ff00; }");
		}
		else
		{
			account_button->setStyleSheet("QPushButton { color: #ff0000; }");
		}
	}

	QWidget* central_widget;
	QVBoxLayout* root_layout;

	QTabWidget* tab_widget;
	QWidget* tab_transaction_table;
	QVBoxLayout* tab0_root_layout;
	QHBoxLayout* total_amount_layout;
	QLabel* total_amount_label;
	QChartView* total_amount_small_chart;
	QTableView* table_view;
	QHBoxLayout* button_layout;
	QPushButton* add_button;
	QPushButton* add_group_button;
	QPushButton* edit_button;
	QPushButton* remove_button;
	QPushButton* filter_button;
	QPushButton* account_button;

	QWidget* tab_total_amount_graph;
	QVBoxLayout* tab1_root_layout;
	TooltipChartView* total_amount_chart;
};
