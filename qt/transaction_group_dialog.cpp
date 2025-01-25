#include "transaction_group_dialog.h"
#include "table_style_delegate.hpp"
#include "total_amount.hpp"
#include "transaction.hpp"
#include "transaction_dialog.h"
#include "util/random_generator.hpp"
#include "validation.hpp"
#include <memory>
#include <qmessagebox.h>
#include <qshortcut.h>

TransactionGroupDialog::TransactionGroupDialog(QWidget *parent)
	: QDialog(parent)
	, ui(TransactionGroupDialogUI())
	, transactionModel()
{
	transactionGroup.id = 0;
	transactionGroup.date = QDate::currentDate();
	transactionGroup.category = transactionCategories.back();
	transactionGroup.amount = 0;
	init();
}

TransactionGroupDialog::TransactionGroupDialog(const TransactionGroup& transactionGroup, QWidget *parent)
	: QDialog(parent)
	, ui(TransactionGroupDialogUI())
	, transactionModel()
	, transactionGroup(transactionGroup)
{
	init();
}

TransactionGroupDialog::~TransactionGroupDialog()
{}

void TransactionGroupDialog::init()
{
	this->setLayout(ui.rootLayout);
	this->setGeometry(QRect(QPoint(0, 0), QPoint(1400, 800)));
	ui.dateInput->setDisplayFormat("dd.MM.yyyy");
	ui.dateInput->setDate(transactionGroup.date);

	ui.categoryInput->addItems(transactionCategories);
	ui.categoryInput->setCurrentIndex(getTransactionCategoryIndex(transactionGroup.getField(TransactionFieldNames::Category)));

	ui.descriptionInput->setPlaceholderText("Enter description...");
	ui.descriptionInput->setText(transactionGroup.getField(TransactionFieldNames::Description));

	QShortcut* addShortcut = new QShortcut(QKeySequence("Ctrl+A"), this);
	connect(addShortcut, &QShortcut::activated, this, &TransactionGroupDialog::openAddTransactionDialog);
	connect(ui.addButton, &QPushButton::clicked, this, &TransactionGroupDialog::openAddTransactionDialog);
	QShortcut* editShortcut = new QShortcut(QKeySequence("Ctrl+E"), this);
	connect(editShortcut, &QShortcut::activated, this, &TransactionGroupDialog::openEditTransactionDialog);
	connect(ui.editButton, &QPushButton::clicked, this, &TransactionGroupDialog::openEditTransactionDialog);
	QShortcut* removeShortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	connect(removeShortcut, &QShortcut::activated, this, &TransactionGroupDialog::openDeleteTransactionDialog);
	connect(ui.removeButton, &QPushButton::clicked, this, &TransactionGroupDialog::openDeleteTransactionDialog);
	QShortcut* okShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(okShortcut, &QShortcut::activated, this, &QDialog::accept);
	connect(ui.okButton, &QPushButton::clicked, this, &QDialog::accept);
	QShortcut* cancelShortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
	connect(cancelShortcut, &QShortcut::activated, this, &QDialog::reject);
	connect(ui.cancelButton, &QPushButton::clicked, this, &QDialog::reject);

	for (const std::shared_ptr<Transaction>& transaction : transactionGroup.transactions) transactionModel.add(transaction);
	ui.tableView->setModel(&transactionModel);
	ui.tableView->resizeColumnsToContents();
	ui.tableView->resizeRowsToContents();
	if (ui.tableView->columnWidth(4) > 800) ui.tableView->setColumnWidth(4, 800);
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView->setItemDelegate(new TableStyleDelegate(ui.tableView));
	ui.totalAmountLabel->setText(getCurrentTotalAmount(transactionModel).toString() + " €");
}

TransactionGroup TransactionGroupDialog::getTransactionGroup()
{
	transactionGroup.transactions.clear();
	transactionGroup.date = ui.dateInput->date();
	transactionGroup.category = ui.categoryInput->currentText();
	transactionGroup.description = ui.descriptionInput->text();
	// prevent overwriting of the id when a transaction is edited
	if (transactionGroup.id == 0) transactionGroup.id = rng::random_int64();
	for (uint32_t i = 0; i < transactionModel.rowCount(); i++) transactionGroup.transactions.push_back(transactionModel.getTransaction(i));
	transactionGroup.amount = getCurrentTotalAmount(transactionModel);
	return transactionGroup;
}

void TransactionGroupDialog::openAddTransactionDialog()
{
	Transaction transaction;
	transaction.date = ui.dateInput->date();
	transaction.category = ui.categoryInput->currentText();
	TransactionDialog dialog(transaction, this);

	if (dialog.exec() == QDialog::Accepted)
	{
		std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(dialog.getTransaction());
		transactionModel.add(transaction);
		ui.update(transactionModel);
	}
}

void TransactionGroupDialog::openEditTransactionDialog()
{
	int32_t idx = ui.tableView->selectionModel()->currentIndex().row();
	if (!validateTransactionIndex(idx, transactionModel, this)) return;
	TransactionDialog dialog(*transactionModel.getTransaction(idx), this);

	if (dialog.exec() == QDialog::Accepted)
	{
		std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(dialog.getTransaction());
		transactionModel.setTransaction(idx, transaction);
		ui.update(transactionModel);
	}
}

void TransactionGroupDialog::openDeleteTransactionDialog()
{
	int32_t idx = ui.tableView->selectionModel()->currentIndex().row();
	if (!validateTransactionIndex(idx, transactionModel, this)) return;
	QString message(QString("Delete transaction %1?").arg(transactionModel.getTransaction(idx)->getField(TransactionFieldNames::ID)));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		transactionModel.removeTransaction(idx);
		ui.update(transactionModel);
	}
}
