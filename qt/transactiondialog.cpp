#include "transactiondialog.h"
#include "transaction.hpp"

TransactionDialog::TransactionDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle("Add Transaction");
	resize(300, 150);

	dateInput = new QDateEdit(this);
	dateInput->setDisplayFormat("dd.MM.yyyy");
	dateInput->setDate(QDate::currentDate());

	categoryInput = new QComboBox(this);
	categoryInput->addItems(categories);

	amountInput = new QLineEdit(this);
	amountInput->setPlaceholderText("Enter amount...");

	descriptionInput = new QLineEdit(this);
	descriptionInput->setPlaceholderText("Enter description...");

	QPushButton *okButton = new QPushButton("OK", this);
	QPushButton *cancelButton = new QPushButton("Cancel", this);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(dateInput);
	mainLayout->addWidget(categoryInput);
	mainLayout->addWidget(amountInput);
	mainLayout->addWidget(descriptionInput);
	mainLayout->addLayout(buttonLayout);

	connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QDate TransactionDialog::getDate() const
{
	return dateInput->date();
}

QString TransactionDialog::getCategory() const
{
	return categoryInput->currentText();
}

QString TransactionDialog::getAmount() const
{
	return amountInput->text();
}

QString TransactionDialog::getDescription() const
{
	return descriptionInput->text();
}
