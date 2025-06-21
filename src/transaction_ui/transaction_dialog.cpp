#include "transaction_ui/transaction_dialog.hpp"
#include "transaction.hpp"
#include "util/random_generator.hpp"
#include <qcompleter.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qshortcut.h>

TransactionDialog::TransactionDialog(const TransactionModel& globalTransactionModel, QWidget *parent) : QDialog(parent), globalTransactionModel(globalTransactionModel)
{
	init();
}

TransactionDialog::TransactionDialog(const TransactionModel& globalTransactionModel, const Transaction& transaction, QWidget *parent) : QDialog(parent), globalTransactionModel(globalTransactionModel), transaction(transaction)
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
	dateInput->installEventFilter(this);

	QCompleter* completer = new QCompleter(globalTransactionModel.getUniqueValueList(TransactionFieldNames::Description), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	descriptionInput = new QLineEdit(this);
	descriptionInput->setCompleter(completer);
	descriptionInput->setPlaceholderText("Enter description...");
	descriptionInput->setText(transaction.getField(TransactionFieldNames::Description));
	connect(descriptionInput, &QLineEdit::editingFinished, this, &TransactionDialog::autoCompleteFromDescription);

	amountInput = new QLineEdit(this);
	amountInput->setPlaceholderText("Enter amount...");
	amountInput->setText(transaction.getField(TransactionFieldNames::Amount));

	categoryInput = new QComboBox(this);
	categoryInput->addItems(globalTransactionModel.getCategoryNames());
	categoryInput->setCurrentText(transaction.category);

	QPushButton *okButton = new QPushButton("OK", this);
	QPushButton *cancelButton = new QPushButton("Cancel", this);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(dateInput);
	mainLayout->addWidget(descriptionInput);
	mainLayout->addWidget(amountInput);
	mainLayout->addWidget(categoryInput);
	mainLayout->addLayout(buttonLayout);

	QShortcut* okShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(okShortcut, &QShortcut::activated, this, &QDialog::accept);
	connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
	QShortcut* cancelShortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
	connect(cancelShortcut, &QShortcut::activated, this, &QDialog::reject);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void TransactionDialog::autoCompleteFromDescription()
{
	std::shared_ptr<const Transaction> completedTransaction;
	if (globalTransactionModel.getAutoCompleteTransaction(descriptionInput->text(), completedTransaction))
	{
		categoryInput->setCurrentText(completedTransaction->category);
		amountInput->setText(completedTransaction->getField(TransactionFieldNames::Amount));
	}
}

bool TransactionDialog::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == dateInput && event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Tab)
		{
			QWidget::focusNextChild();
			return true;
		}
		else if (keyEvent->key() == Qt::Key_L)
		{
			QKeyEvent rightArrowEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ControlModifier);
			QCoreApplication::sendEvent(dateInput, &rightArrowEvent);
			return true;
		}
		else if (keyEvent->key() == Qt::Key_H)
		{
			QKeyEvent rightArrowEvent(QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier);
			QCoreApplication::sendEvent(dateInput, &rightArrowEvent);
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
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
