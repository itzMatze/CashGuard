#include "transactiondialog.h"
#include "transaction.hpp"
#include "util/random_generator.hpp"

TransactionDialog::TransactionDialog(QWidget *parent) : QDialog(parent)
{
	transaction.date = QDate::currentDate();
	transaction.category = transactionCategories.back();
	transaction.id = 0;
	init();
}

TransactionDialog::TransactionDialog(const Transaction& transaction, QWidget *parent) : transaction(transaction)
{
	init();
}

void TransactionDialog::init()
{
	setWindowTitle("Add Transaction");
	resize(300, 150);

	dateInput = new QDateEdit(this);
	dateInput->setDisplayFormat("dd.MM.yyyy");
	dateInput->setDate(transaction.date);

	categoryInput = new QComboBox(this);
	categoryInput->addItems(transactionCategories);
	categoryInput->setCurrentIndex(getTransactionCategoryIndex(transaction.getField(TransactionFieldNames::Category)));

	amountInput = new QLineEdit(this);
	amountInput->setPlaceholderText("Enter amount...");
	amountInput->setText(transaction.getField(TransactionFieldNames::Amount));

	descriptionInput = new QLineEdit(this);
	descriptionInput->setPlaceholderText("Enter description...");
	descriptionInput->setText(transaction.getField(TransactionFieldNames::Description));

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

Transaction TransactionDialog::getTransaction() const
{
	Transaction transaction;
	transaction.date = dateInput->date();
	transaction.category = categoryInput->currentText();
	transaction.amount = Amount{amountInput->text()};
	transaction.description = descriptionInput->text();
	// prevent overwriting of the id when a transaction is edited
	if (transaction.id == 0) transaction.id = rng::random_int32();
	return transaction;
}
