#pragma once

#include "account_model.hpp"
#include "qt_util.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtableview.h>
#include <qwidget.h>

class AccountDialogUI
{
public:
	explicit AccountDialogUI(const AccountModel& accountModel, const Amount& totalAmount, QWidget* parent = nullptr) :
		rootLayout(new QVBoxLayout(parent)),
		tableView(new QTableView(parent)),
		addButton(new QPushButton(parent)),
		removeButton(new QPushButton(parent)),
		quitButton(new QPushButton(parent)),
		buttonLayout(new QHBoxLayout(parent)),
		matchLabel(new QLabel(parent))
	{
		addButton->setFont(setFontSize(12, false, addButton->font()));
		addButton->setText("Add");
		removeButton->setFont(setFontSize(12, false, removeButton->font()));
		removeButton->setText("Remove");
		quitButton->setFont(setFontSize(12, false, quitButton->font()));
		quitButton->setText("Quit");
		buttonLayout->addWidget(addButton);
		buttonLayout->addWidget(removeButton);
		buttonLayout->addWidget(quitButton);
		matchLabel->setFont(setFontSize(20, true, matchLabel->font()));
		rootLayout->addWidget(matchLabel);
		rootLayout->addWidget(tableView);
		rootLayout->addLayout(buttonLayout);
		update(accountModel, totalAmount);
	}

	void update(const AccountModel& accountModel, const Amount& totalAmount)
	{
		Amount accountTotalAmount = accountModel.getTotalAmount();
		if (accountTotalAmount.value == totalAmount.value)
		{
			matchLabel->setText("Amounts match!");
			matchLabel->setStyleSheet("QLabel { color: #00ff00; }");
		}
		else
		{
			matchLabel->setText("Amounts don't match! Accounts: " + accountTotalAmount.toString() + " €, Difference: " + Amount(accountTotalAmount.value - totalAmount.value).toString() + " €");
			matchLabel->setStyleSheet("QLabel { color: #ff0000; }");
		}
	}

	QVBoxLayout* rootLayout;
	QTableView* tableView;
	QHBoxLayout* buttonLayout;
	QPushButton* addButton;
	QPushButton* removeButton;
	QPushButton* quitButton;
	QLabel* matchLabel;
};
