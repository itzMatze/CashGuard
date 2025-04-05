#include "transaction_filter_ui/transaction_filter_window.hpp"
#include "transaction.hpp"
#include <qcompleter.h>

TransactionFilterWindow::TransactionFilterWindow(TransactionModel& globalTransactionModel, QWidget* parent) :
	QWidget(parent),
	ui(globalTransactionModel, this),
	globalTransactionModel(globalTransactionModel)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(ui.centralWidget);
	setWindowTitle("Transaction Filter Window");
	resize(500, 150);

	QShortcut* okShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(okShortcut, &QShortcut::activated, this, &TransactionFilterWindow::apply);
	connect(ui.okButton, &QPushButton::clicked, this, &TransactionFilterWindow::apply);
	QShortcut* resetShortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
	connect(resetShortcut, &QShortcut::activated, this, &TransactionFilterWindow::reset);
	connect(ui.resetButton, &QPushButton::clicked, this, &TransactionFilterWindow::reset);

	QCompleter* completer = new QCompleter(globalTransactionModel.getUniqueValueList(TransactionFieldNames::Description), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	ui.descriptionInput->setCompleter(completer);

	ui.update(transactionFilter);
}

void TransactionFilterWindow::apply()
{
	bool valid = true;
	if (ui.dateMinInput->date() > ui.dateMaxInput->date()) valid = false;
	if (Amount{ui.amountMinInput->text()} > Amount{ui.amountMaxInput->text()}) valid = false;
	if (!valid)
	{
		QMessageBox::warning(this, "Invalid Input", "Please correct the inputs.");
		return;
	}
	transactionFilter.negated = ui.negatedCheckBox->isChecked();
	transactionFilter.dateMin = ui.dateMinInput->date();
	transactionFilter.dateMax = ui.dateMaxInput->date();
	transactionFilter.category = ui.categoryInput->currentText();
	transactionFilter.amountMin = (ui.amountMinInput->text().size() > 0) ? Amount{ui.amountMinInput->text()} : Amount(std::numeric_limits<int32_t>::min());
	transactionFilter.amountMax = (ui.amountMaxInput->text().size() > 0) ? Amount{ui.amountMaxInput->text()} : Amount(std::numeric_limits<int32_t>::max());
	transactionFilter.searchPhrase = ui.descriptionInput->text();
	transactionFilter.active = true;
	globalTransactionModel.setFilter(transactionFilter);
	emit updateMainUI();
}

void TransactionFilterWindow::reset()
{
	transactionFilter = TransactionFilter();
	transactionFilter.dateMax = globalTransactionModel.getUnfilteredTransactions().at(0)->date;
	transactionFilter.dateMin = globalTransactionModel.getUnfilteredTransactions().back()->date;
	ui.update(transactionFilter);
	emit updateMainUI();
}
