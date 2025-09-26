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
	explicit MainWindowUI(QWidget* parent = nullptr) :
		centralWidget(new QWidget(parent)),
		rootLayout(new QVBoxLayout(parent)),
		tabWidget(new QTabWidget(parent)),
		tabTransactionTable(new QWidget(parent)),
		tab0RootLayout(new QVBoxLayout(parent)),
		totalAmountLayout(new QHBoxLayout(parent)),
		totalAmountLabel(new QLabel(parent)),
		totalAmountSmallChart(new QChartView(parent)),
		tableView(new QTableView(parent)),
		buttonLayout(new QHBoxLayout(parent)),
		addButton(new QPushButton(parent)),
		addGroupButton(new QPushButton(parent)),
		editButton(new QPushButton(parent)),
		removeButton(new QPushButton(parent)),
		filterButton(new QPushButton(parent)),
		accountButton(new QPushButton(parent)),
		tabTotalAmountGraph(new QWidget(parent)),
		tab1RootLayout(new QVBoxLayout(parent)),
		totalAmountChart(new TooltipChartView(parent))
	{
		centralWidget->setLayout(rootLayout);
		rootLayout->addWidget(tabWidget);

		// tab 0
		tabWidget->addTab(tabTransactionTable, "Transaction Table");
		tabTransactionTable->setLayout(tab0RootLayout);
		// total amount value and graph
		totalAmountLabel->setFont(setFontSize(20, true, totalAmountLabel->font()));
		totalAmountLabel->setIndent(20);
		totalAmountLayout->addWidget(totalAmountLabel);
		totalAmountSmallChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		totalAmountSmallChart->setMaximumHeight(200);
		totalAmountSmallChart->setRenderHint(QPainter::Antialiasing);
		totalAmountLayout->addWidget(totalAmountSmallChart);
		tab0RootLayout->addLayout(totalAmountLayout);
		// transaction table
		tableView->setFont(setFontSize(12, false, tableView->font()));
		tab0RootLayout->addWidget(tableView);
		tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
		tableView->setSelectionMode(QAbstractItemView::SingleSelection);
		tableView->setItemDelegate(new TableStyleDelegate(tableView));
		// buttons
		addButton->setFont(setFontSize(12, false, addButton->font()));
		addButton->setText("Add");
		addGroupButton->setFont(setFontSize(12, false, addGroupButton->font()));
		addGroupButton->setText("Add Group");
		editButton->setFont(setFontSize(12, false, editButton->font()));
		editButton->setText("Edit");
		removeButton->setFont(setFontSize(12, false, removeButton->font()));
		removeButton->setText("Remove");
		filterButton->setFont(setFontSize(12, false, filterButton->font()));
		filterButton->setText("Filter");
		accountButton->setFont(setFontSize(12, false, accountButton->font()));
		accountButton->setText("Accounts");
		buttonLayout->addWidget(addButton);
		buttonLayout->addWidget(addGroupButton);
		buttonLayout->addWidget(editButton);
		buttonLayout->addWidget(removeButton);
		buttonLayout->addWidget(filterButton);
		buttonLayout->addWidget(accountButton);
		tab0RootLayout->addLayout(buttonLayout);

		// tab 1
		tabWidget->addTab(tabTotalAmountGraph, "Total Amount Graph");
		tabTotalAmountGraph->setLayout(tab1RootLayout);
		totalAmountChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		totalAmountChart->setRenderHint(QPainter::Antialiasing);
		tab1RootLayout->addWidget(totalAmountChart);
	}

	void update(const TransactionModel& transactionModel, const AccountModel& accountModel, const Amount& filteredTotalAmount, const Amount& globalTotalAmount)
	{
		totalAmountLabel->setText(filteredTotalAmount.toString() + " €");
		totalAmountSmallChart->setChart(getSmallTotalAmountChart(transactionModel).first);
		std::pair<QChart*, QLineSeries*> totalAmountData = getTotalAmountChart(transactionModel);
		totalAmountChart->updateChart(totalAmountData.first, totalAmountData.second, transactionModel.getFilter().dateMin);
		if (accountModel.getTotalAmount().value == globalTotalAmount.value)
		{
			accountButton->setStyleSheet("QPushButton { color: #00ff00; }");
		}
		else
		{
			accountButton->setStyleSheet("QPushButton { color: #ff0000; }");
		}
	}

	QWidget* centralWidget;
	QVBoxLayout* rootLayout;

	QTabWidget* tabWidget;
	QWidget* tabTransactionTable;
	QVBoxLayout* tab0RootLayout;
	QHBoxLayout* totalAmountLayout;
	QLabel* totalAmountLabel;
	QChartView* totalAmountSmallChart;
	QTableView* tableView;
	QHBoxLayout* buttonLayout;
	QPushButton* addButton;
	QPushButton* addGroupButton;
	QPushButton* editButton;
	QPushButton* removeButton;
	QPushButton* filterButton;
	QPushButton* accountButton;

	QWidget* tabTotalAmountGraph;
	QVBoxLayout* tab1RootLayout;
	TooltipChartView* totalAmountChart;
};
