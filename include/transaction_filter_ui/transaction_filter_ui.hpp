#pragma once

#include "transaction_filter.hpp"
#include "transaction_model.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qwidget.h>

class TransactionFilterUI
{
public:
	TransactionFilterUI(const TransactionModel& globalTransactionModel, QWidget* parent) :
		centralWidget(new QWidget(parent)),
		negatedCheckBox(new QCheckBox(parent)),
		dateMinLabel(new QLabel(parent)),
		dateMinInput(new QDateEdit(parent)),
		dateMaxLabel(new QLabel(parent)),
		dateMaxInput(new QDateEdit(parent)),
		categoryLabel(new QLabel(parent)),
		categoryInput(new QComboBox(parent)),
		amountMinLabel(new QLabel(parent)),
		amountMinInput(new QLineEdit(parent)),
		amountMaxLabel(new QLabel(parent)),
		amountMaxInput(new QLineEdit(parent)),
		descriptionLabel(new QLabel(parent)),
		descriptionInput(new QLineEdit(parent)),
		okButton(new QPushButton(parent)),
		resetButton(new QPushButton(parent)),
		globalTransactionModel(globalTransactionModel)
	{
		dateMinLabel->setText("Min Date");
		dateMinInput->setDisplayFormat("dd.MM.yyyy");
		dateMaxLabel->setText("Max Date");
		dateMaxInput->setDisplayFormat("dd.MM.yyyy");
		categoryLabel->setText("Category (None to disable filtering)");
		categoryInput->addItems(globalTransactionModel.getCategoryNames());
		amountMinLabel->setText("Min Amount");
		amountMinInput->setPlaceholderText("Enter amount...");
		amountMaxLabel->setText("Max Amount");
		amountMaxInput->setPlaceholderText("Enter amount...");
		descriptionLabel->setText("Search Phrase");
		descriptionInput->setPlaceholderText("Enter phrase...");
		negatedCheckBox->setText("Negate Filter");

		okButton->setText("OK");
		resetButton->setText("Reset");
		QHBoxLayout* buttonLayout = new QHBoxLayout();
		buttonLayout->addWidget(okButton);
		buttonLayout->addWidget(resetButton);

		QHBoxLayout* dateLabelLayout = new QHBoxLayout();
		dateLabelLayout->addWidget(dateMinLabel);
		dateLabelLayout->addWidget(dateMaxLabel);
		QHBoxLayout* dateLayout = new QHBoxLayout();
		dateLayout->addWidget(dateMinInput);
		dateLayout->addWidget(dateMaxInput);

		QHBoxLayout* amountLabelLayout = new QHBoxLayout();
		amountLabelLayout->addWidget(amountMinLabel);
		amountLabelLayout->addWidget(amountMaxLabel);
		QHBoxLayout* amountLayout = new QHBoxLayout();
		amountLayout->addWidget(amountMinInput);
		amountLayout->addWidget(amountMaxInput);

		QVBoxLayout* rootLayout = new QVBoxLayout();
		rootLayout->addWidget(negatedCheckBox);
		rootLayout->addLayout(dateLabelLayout);
		rootLayout->addLayout(dateLayout);
		rootLayout->addWidget(categoryLabel);
		rootLayout->addWidget(categoryInput);
		rootLayout->addLayout(amountLabelLayout);
		rootLayout->addLayout(amountLayout);
		rootLayout->addWidget(descriptionLabel);
		rootLayout->addWidget(descriptionInput);
		rootLayout->addLayout(buttonLayout);
		centralWidget->setLayout(rootLayout);
	}

	void update(const TransactionFilter& transactionFilter)
	{
		dateMinInput->setDate(transactionFilter.dateMin);
		dateMaxInput->setDate(transactionFilter.dateMax);
		categoryInput->setCurrentText(transactionFilter.category);
		if (transactionFilter.amountMin.value != std::numeric_limits<int32_t>::min()) amountMinInput->setText(transactionFilter.amountMin.toString());
		else amountMinInput->setText("");
		if (transactionFilter.amountMax.value != std::numeric_limits<int32_t>::max()) amountMaxInput->setText(transactionFilter.amountMax.toString());
		else amountMaxInput->setText("");
		descriptionInput->setText(transactionFilter.searchPhrase);
	}

	QWidget* centralWidget;
	QCheckBox* negatedCheckBox;
	QLabel* dateMinLabel;
	QDateEdit* dateMinInput;
	QLabel* dateMaxLabel;
	QDateEdit* dateMaxInput;
	QLabel* categoryLabel;
	QComboBox* categoryInput;
	QLabel* amountMinLabel;
	QLineEdit* amountMinInput;
	QLabel* amountMaxLabel;
	QLineEdit* amountMaxInput;
	QLabel* descriptionLabel;
	QLineEdit* descriptionInput;
	QPushButton* okButton;;
	QPushButton* resetButton;

private:
	const TransactionModel& globalTransactionModel;
};
