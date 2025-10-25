#include "transaction_filter_ui/transaction_filter_window.hpp"
#include "transaction.hpp"
#include <qcompleter.h>

TransactionFilterWindow::TransactionFilterWindow(TransactionModel& global_transaction_model, QWidget* parent) :
	QWidget(parent),
	ui(global_transaction_model, this),
	global_transaction_model(global_transaction_model)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(ui.central_widget);
	setWindowTitle("Transaction Filter Window");
	resize(500, 150);

	QShortcut* ok_shortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(ok_shortcut, &QShortcut::activated, this, &TransactionFilterWindow::apply);
	connect(ui.ok_button, &QPushButton::clicked, this, &TransactionFilterWindow::apply);
	QShortcut* reset_shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
	connect(reset_shortcut, &QShortcut::activated, this, &TransactionFilterWindow::reset);
	connect(ui.reset_button, &QPushButton::clicked, this, &TransactionFilterWindow::reset);

	QCompleter* completer = new QCompleter(global_transaction_model.get_unique_value_list(TransactionFieldNames::Description), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	ui.description_input->setCompleter(completer);

	reset();
}

void TransactionFilterWindow::apply()
{
	bool valid = true;
	if (ui.date_min_input->date() > ui.date_max_input->date()) valid = false;
	if (Amount{ui.amount_min_input->text()} > Amount{ui.amount_max_input->text()}) valid = false;
	if (!valid)
	{
		QMessageBox::warning(this, "Invalid Input", "Please correct the inputs.");
		return;
	}
	transaction_filter.negated = ui.negated_check_box->isChecked();
	transaction_filter.date_min = ui.date_min_input->date();
	transaction_filter.date_max = ui.date_max_input->date();
	transaction_filter.category = ui.category_lnput->currentText();
	transaction_filter.amount_min = (ui.amount_min_input->text().size() > 0) ? Amount{ui.amount_min_input->text()} : Amount(std::numeric_limits<int32_t>::min());
	transaction_filter.amount_max = (ui.amount_max_input->text().size() > 0) ? Amount{ui.amount_max_input->text()} : Amount(std::numeric_limits<int32_t>::max());
	transaction_filter.search_phrase = ui.description_input->text();
	transaction_filter.active = true;
	global_transaction_model.set_filter(transaction_filter);
	emit update_main_ui();
}

void TransactionFilterWindow::reset()
{
	transaction_filter = TransactionFilter();
	if (!global_transaction_model.is_empty())
	{
		transaction_filter.date_max = global_transaction_model.get_unfiltered_transactions().at(0)->date;
		transaction_filter.date_min = global_transaction_model.get_unfiltered_transactions().back()->date;
	}
	ui.update(transaction_filter);
	emit update_main_ui();
}
