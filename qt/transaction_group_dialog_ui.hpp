#pragma once

#include "total_amount.hpp"
#include "transaction_model.hpp"
#include "qt_util.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtableview.h>
#include <qwidget.h>

class TransactionGroupDialogUI
{
public:
	explicit TransactionGroupDialogUI(QWidget* parent = nullptr) :
		rootLayout(new QVBoxLayout(parent)),
		totalAmountLabel(new QLabel(parent)),
		dateInput(new QDateEdit(parent)),
		categoryInput(new QComboBox(parent)),
		descriptionInput(new QLineEdit(parent)),
		tableView(new QTableView(parent)),
		transactionButtonLayout(new QHBoxLayout(parent)),
		addButton(new QPushButton(parent)),
		editButton(new QPushButton(parent)),
		removeButton(new QPushButton(parent)),
		confirmationButtonLayout(new QHBoxLayout(parent)),
		okButton(new QPushButton(parent)),
		cancelButton(new QPushButton(parent))
	{
		totalAmountLabel->setFont(setFontSize(20, true, totalAmountLabel->font()));
		rootLayout->addWidget(totalAmountLabel);
		rootLayout->addWidget(dateInput);
		rootLayout->addWidget(categoryInput);
		rootLayout->addWidget(descriptionInput);
		rootLayout->addWidget(tableView);

		addButton->setFont(setFontSize(12, false, addButton->font()));
		addButton->setText("Add");
		editButton->setFont(setFontSize(12, false, editButton->font()));
		editButton->setText("Edit");
		removeButton->setFont(setFontSize(12, false, removeButton->font()));
		removeButton->setText("Remove");
		transactionButtonLayout->addWidget(addButton);
		transactionButtonLayout->addWidget(editButton);
		transactionButtonLayout->addWidget(removeButton);
		rootLayout->addLayout(transactionButtonLayout);

		okButton->setFont(setFontSize(12, false, okButton->font()));
		okButton->setText("OK");
		cancelButton->setFont(setFontSize(12, false, cancelButton->font()));
		cancelButton->setText("Cancel");
		confirmationButtonLayout->addWidget(okButton);
		confirmationButtonLayout->addWidget(cancelButton);
		rootLayout->addLayout(confirmationButtonLayout);
	}

	void update(const TransactionModel& transactionModel)
	{
		Amount totalAmount = getCurrentTotalAmount(transactionModel);
		totalAmountLabel->setText(totalAmount.toString());
	}

	QVBoxLayout* rootLayout;
	QLabel* totalAmountLabel;
	QDateEdit* dateInput;
	QComboBox* categoryInput;
	QLineEdit* descriptionInput;
	QTableView* tableView;

	QHBoxLayout* transactionButtonLayout;
	QPushButton* addButton;
	QPushButton* editButton;
	QPushButton* removeButton;

	QHBoxLayout* confirmationButtonLayout;
	QPushButton* okButton;
	QPushButton* cancelButton;
};
