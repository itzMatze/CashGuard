#include "transaction_ui/transaction_dialog.hpp"
#include "transaction.hpp"
#include "util/random_generator.hpp"
#include <qcompleter.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qshortcut.h>

TransactionDialog::TransactionDialog(const TransactionModel& global_transaction_model, QWidget* parent)
	: QDialog(parent)
	, global_transaction_model(global_transaction_model)
{
	init();
}

TransactionDialog::TransactionDialog(const TransactionModel& global_transaction_model, const Transaction& transaction, QWidget* parent)
	: QDialog(parent)
	, global_transaction_model(global_transaction_model)
	, transaction(transaction)
{
	init();
}

void TransactionDialog::init()
{
	setWindowTitle("Transaction Dialog");
	resize(300, 150);

	date_input = new QDateEdit(this);
	date_input->setDisplayFormat("dd.MM.yyyy");
	date_input->setDate(transaction.date);
	date_input->installEventFilter(this);

	QCompleter* completer = new QCompleter(global_transaction_model.get_unique_value_list(TransactionFieldNames::Description), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	description_input = new QLineEdit(this);
	description_input->setCompleter(completer);
	description_input->setPlaceholderText("Enter description...");
	description_input->setText(transaction.get_field(TransactionFieldNames::Description));
	connect(description_input, &QLineEdit::editingFinished, this, &TransactionDialog::auto_complete_from_description);

	amount_input = new QLineEdit(this);
	amount_input->setPlaceholderText("Enter amount...");
	amount_input->setText(transaction.get_field(TransactionFieldNames::Amount));

	category_input = new QComboBox(this);
	category_input->addItems(global_transaction_model.get_category_names());
	category_input->setCurrentText(transaction.category);

	QPushButton* ok_button = new QPushButton("OK", this);
	QPushButton* cancel_button = new QPushButton("Cancel", this);

	QHBoxLayout* button_layout = new QHBoxLayout();
	button_layout->addWidget(ok_button);
	button_layout->addWidget(cancel_button);

	QVBoxLayout* main_layout = new QVBoxLayout(this);
	main_layout->addWidget(date_input);
	main_layout->addWidget(description_input);
	main_layout->addWidget(amount_input);
	main_layout->addWidget(category_input);
	main_layout->addLayout(button_layout);

	QShortcut* ok_shortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(ok_shortcut, &QShortcut::activated, this, &QDialog::accept);
	connect(ok_button, &QPushButton::clicked, this, &QDialog::accept);
	QShortcut* cancel_shortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
	connect(cancel_shortcut, &QShortcut::activated, this, &QDialog::reject);
	connect(cancel_button, &QPushButton::clicked, this, &QDialog::reject);
}

void TransactionDialog::auto_complete_from_description()
{
	if (!amount_input->text().isEmpty() && Amount(amount_input->text()).value != 0) return;
	std::shared_ptr<const Transaction> completed_transaction;
	if (global_transaction_model.get_auto_complete_transaction(description_input->text(), completed_transaction))
	{
		category_input->setCurrentText(completed_transaction->category);
		amount_input->setText(completed_transaction->get_field(TransactionFieldNames::Amount));
	}
}

bool TransactionDialog::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == date_input && event->type() == QEvent::KeyPress)
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
		if (key_event->key() == Qt::Key_Tab)
		{
			QWidget::focusNextChild();
			return true;
		}
		else if (key_event->key() == Qt::Key_L)
		{
			QKeyEvent right_arrow_event(QEvent::KeyPress, Qt::Key_Right, Qt::ControlModifier);
			QCoreApplication::sendEvent(date_input, &right_arrow_event);
			return true;
		}
		else if (key_event->key() == Qt::Key_H)
		{
			QKeyEvent right_arrow_event(QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier);
			QCoreApplication::sendEvent(date_input, &right_arrow_event);
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

Transaction TransactionDialog::get_transaction()
{
	transaction.date = date_input->date();
	transaction.category = category_input->currentText();
	transaction.amount = Amount{amount_input->text()};
	transaction.description = description_input->text();
	// prevent overwriting of the id when a transaction is edited
	if (transaction.id == 0) transaction.id = rng::random_int64();
	return transaction;
}
