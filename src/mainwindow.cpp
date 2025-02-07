#include "mainwindow.hpp"
#include "transaction_filter_dialog.hpp"
#include "transaction_model.hpp"
#include "transaction_file_handler.hpp"
#include "mainwindow_ui.hpp"
#include "transaction_dialog.hpp"
#include "transaction_group_dialog.hpp"
#include "validation.hpp"
#include <QFileDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QShortcut>
#include <QTextStream>
#include <memory>
#include <qtablewidget.h>

MainWindow::MainWindow(const QString& filePath, QWidget *parent)
	: QMainWindow(parent)
	, ui(MainWindowUI())
	, transactionModel()
	, filePath(filePath)
{
	setWindowTitle("Cash Guard");
	setCentralWidget(ui.centralWidget);
	QShortcut* addShortcut = new QShortcut(QKeySequence("Ctrl+A"), this);
	connect(addShortcut, &QShortcut::activated, this, &MainWindow::openAddTransactionDialog);
	connect(ui.addButton, &QPushButton::clicked, this, &MainWindow::openAddTransactionDialog);
	QShortcut* addGroupShortcut = new QShortcut(QKeySequence("Ctrl+G"), this);
	connect(addGroupShortcut, &QShortcut::activated, this, &MainWindow::openAddTransactionGroupDialog);
	connect(ui.addGroupButton, &QPushButton::clicked, this, &MainWindow::openAddTransactionGroupDialog);
	QShortcut* editShortcut = new QShortcut(QKeySequence("Ctrl+E"), this);
	connect(editShortcut, &QShortcut::activated, this, &MainWindow::openEditTransactionDialog);
	connect(ui.editButton, &QPushButton::clicked, this, &MainWindow::openEditTransactionDialog);
	QShortcut* removeShortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	connect(removeShortcut, &QShortcut::activated, this, &MainWindow::openDeleteTransactionDialog);
	connect(ui.removeButton, &QPushButton::clicked, this, &MainWindow::openDeleteTransactionDialog);
	QShortcut* filterShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
	connect(filterShortcut, &QShortcut::activated, this, &MainWindow::openFilterDialog);

	if (!loadFromFile(filePath, transactionModel)) QMessageBox::warning(this, "Error", "Failed to load data!");
	transactionModel.getFilter().dateMax = transactionModel.getUnfilteredTransactions().at(0)->date;
	transactionModel.getFilter().dateMin = transactionModel.getUnfilteredTransactions().back()->date;
	ui.tableView->setModel(&transactionModel);
	ui.tableView->resizeColumnsToContents();
	if (ui.tableView->columnWidth(4) > 800) ui.tableView->setColumnWidth(4, 800);

	ui.update(transactionModel);
}

MainWindow::~MainWindow()
{}

void MainWindow::openAddTransactionDialog()
{
	TransactionDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(dialog.getTransaction());
		transaction->added = QDateTime::currentDateTime();
		transaction->edited = QDateTime::currentDateTime();
		transactionModel.add(transaction);
		ui.update(transactionModel);
		saveTransactions();
	}
}

void MainWindow::openAddTransactionGroupDialog()
{
	TransactionGroupDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		std::shared_ptr<TransactionGroup> transaction = std::make_shared<TransactionGroup>(dialog.getTransactionGroup());
		transaction->added = QDateTime::currentDateTime();
		transaction->edited = QDateTime::currentDateTime();
		transactionModel.add(transaction);
		ui.update(transactionModel);
		saveTransactions();
	}
}

void MainWindow::openEditTransactionDialog()
{
	int32_t idx = ui.tableView->selectionModel()->currentIndex().row();
	if (!validateTransactionIndex(idx, transactionModel, this)) return;
	std::shared_ptr<Transaction> transaction = transactionModel.getTransaction(idx);
	if (std::shared_ptr<TransactionGroup> transactionGroup = std::dynamic_pointer_cast<TransactionGroup>(transaction))
	{
		TransactionGroupDialog dialog(*transactionGroup, this);
		if (dialog.exec() == QDialog::Accepted)
		{
			std::shared_ptr<TransactionGroup> newTransactionGroup = std::make_shared<TransactionGroup>(dialog.getTransactionGroup());
			if (*transactionGroup == *newTransactionGroup) return;
			newTransactionGroup->edited = QDateTime::currentDateTime();
			transactionModel.setTransaction(idx, newTransactionGroup);
		}
	}
	else
	{
		TransactionDialog dialog(*transaction, this);
		if (dialog.exec() == QDialog::Accepted)
		{
			std::shared_ptr<Transaction> newTransaction = std::make_shared<Transaction>(dialog.getTransaction());
			if (*transaction == *newTransaction) return;
			newTransaction->edited = QDateTime::currentDateTime();
			transactionModel.setTransaction(idx, newTransaction);
		}
	}
	ui.update(transactionModel);
	saveTransactions();
}

void MainWindow::openDeleteTransactionDialog()
{
	int32_t idx = ui.tableView->selectionModel()->currentIndex().row();
	if (!validateTransactionIndex(idx, transactionModel, this)) return;
	QString message(QString("Delete transaction %1?").arg(transactionModel.getTransaction(idx)->getField(TransactionFieldNames::ID)));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		transactionModel.removeTransaction(idx);
		ui.update(transactionModel);
		saveTransactions();
	}
}

void MainWindow::openFilterDialog()
{
	TransactionFilterDialog dialog(transactionModel, this);

	if (dialog.exec() == QDialog::Accepted)
	{
		transactionModel.setFilter(dialog.getTransactionFilter());
		ui.update(transactionModel);
	}
}

void MainWindow::saveTransactions()
{
	if (!saveToFile(filePath, transactionModel)) QMessageBox::warning(this, "Error", "Failed to save data!");
}
