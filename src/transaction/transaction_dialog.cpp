#include "transaction/transaction_dialog.hpp"
#include "transaction.hpp"
#include "transaction_model.hpp"
#include "util/random_generator.hpp"
#include <QComboBox>
#include <QCompleter>
#include <QDateEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QShortcut>

TransactionDialog::TransactionDialog(const TransactionModel& global_transaction_model, QWidget* parent)
	: QDialog(parent)
	, global_transaction_model(global_transaction_model)
	, ui(this)
{
	init();
}

TransactionDialog::TransactionDialog(const TransactionModel& global_transaction_model, const Transaction& transaction, QWidget* parent)
	: QDialog(parent)
	, global_transaction_model(global_transaction_model)
	, transaction(transaction)
	, ui(this)
{
	init();
}

void TransactionDialog::init()
{
	setWindowTitle("Transaction Dialog");
	resize(300, 150);

	ui.date_input->installEventFilter(this);
	QCompleter* completer = new QCompleter(global_transaction_model.get_unique_value_list(TransactionFieldNames::Description), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	ui.description_input->setCompleter(completer);
	connect(ui.description_input, &QLineEdit::editingFinished, this, &TransactionDialog::auto_complete_from_description);
	ui.category_input->addItems(global_transaction_model.get_category_names());

	QShortcut* ok_shortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(ok_shortcut, &QShortcut::activated, this, &QDialog::accept);
	connect(ui.ok_button, &QPushButton::clicked, this, &QDialog::accept);
	QShortcut* cancel_shortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
	connect(cancel_shortcut, &QShortcut::activated, this, &QDialog::reject);
	connect(ui.cancel_button, &QPushButton::clicked, this, &QDialog::reject);

	ui.update(transaction);
}

void TransactionDialog::auto_complete_from_description()
{
	if (!ui.amount_input->text().isEmpty() && Amount(ui.amount_input->text()).value != 0) return;
	std::shared_ptr<const Transaction> completed_transaction;
	if (global_transaction_model.get_auto_complete_transaction(ui.description_input->text(), completed_transaction))
	{
		ui.category_input->setCurrentText(completed_transaction->category);
		ui.amount_input->setText(completed_transaction->get_field(TransactionFieldNames::Amount));
	}
}

bool TransactionDialog::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == ui.date_input && event->type() == QEvent::KeyPress)
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
			QCoreApplication::sendEvent(ui.date_input, &right_arrow_event);
			return true;
		}
		else if (key_event->key() == Qt::Key_H)
		{
			QKeyEvent right_arrow_event(QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier);
			QCoreApplication::sendEvent(ui.date_input, &right_arrow_event);
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

Transaction TransactionDialog::get_transaction()
{
	transaction.date = ui.date_input->date();
	transaction.category = ui.category_input->currentText();
	transaction.amount = Amount{ui.amount_input->text()};
	transaction.description = ui.description_input->text();
	// prevent overwriting of the id when a transaction is edited
	if (transaction.id == 0) transaction.id = rng::random_int64();
	return transaction;
}
