#pragma once

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
		tabTotalAmountGraph(new QWidget(parent)),
		tab1RootLayout(new QVBoxLayout(parent))
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
		// buttons
		addButton->setFont(setFontSize(12, false, addButton->font()));
		addButton->setText("Add");
		addGroupButton->setFont(setFontSize(12, false, addGroupButton->font()));
		addGroupButton->setText("Add Group");
		editButton->setFont(setFontSize(12, false, editButton->font()));
		editButton->setText("Edit");
		removeButton->setFont(setFontSize(12, false, removeButton->font()));
		removeButton->setText("Remove");
		buttonLayout->addWidget(addButton);
		buttonLayout->addWidget(addGroupButton);
		buttonLayout->addWidget(editButton);
		buttonLayout->addWidget(removeButton);
		tab0RootLayout->addLayout(buttonLayout);

		// tab 1
		tabWidget->addTab(tabTotalAmountGraph, "Total Amount Graph");
		tabTotalAmountGraph->setLayout(tab1RootLayout);
	}

	void update(const TransactionModel& transactionModel)
	{
		totalAmountLabel->setText(getCurrentTotalAmount(transactionModel).toString() + " €");
		totalAmountSmallChart->setChart(getSmallTotalAmountChart(transactionModel).first);
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

	QWidget* tabTotalAmountGraph;
	QVBoxLayout* tab1RootLayout;
};
