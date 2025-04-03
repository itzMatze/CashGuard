#include "transaction_dialog.hpp"
#include "transaction.hpp"
#include "util/random_generator.hpp"
#include <qcompleter.h>
#include <qshortcut.h>

TransactionDialog::TransactionDialog(QWidget *parent) : QDialog(parent)
{
	init();
}

TransactionDialog::TransactionDialog(const Transaction& transaction, QWidget *parent) : transaction(transaction)
{
	init();
}

void TransactionDialog::init()
{
	setWindowTitle("Transaction Dialog");
	resize(300, 150);

	dateInput = new QDateEdit(this);
	dateInput->setDisplayFormat("dd.MM.yyyy");
	dateInput->setDate(transaction.date);

	categoryInput = new QComboBox(this);
	categoryInput->addItems(Category::getCategoryNames());
	categoryInput->setCurrentIndex(transaction.category.getType());

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

	QShortcut* okShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(okShortcut, &QShortcut::activated, this, &QDialog::accept);
	connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
	QShortcut* cancelShortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
	connect(cancelShortcut, &QShortcut::activated, this, &QDialog::reject);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void TransactionDialog::setRecommender(const QStringList& recommendations)
{
	QCompleter* completer = new QCompleter(recommendations, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	descriptionInput->setCompleter(completer);
}

Transaction TransactionDialog::getTransaction()
{
	transaction.date = dateInput->date();
	transaction.category = categoryInput->currentText();
	transaction.amount = Amount{amountInput->text()};
	transaction.description = descriptionInput->text();
	// prevent overwriting of the id when a transaction is edited
	if (transaction.id == 0) transaction.id = rng::random_int64();
	return transaction;
}
