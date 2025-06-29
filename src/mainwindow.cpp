#include "mainwindow.hpp"
#include "transaction.hpp"
#include "transaction_filter_ui/transaction_filter_window.hpp"
#include "transaction_model.hpp"
#include "transaction_file_handler.hpp"
#include "mainwindow_ui.hpp"
#include "transaction_ui/transaction_dialog.hpp"
#include "transaction_ui/transaction_group_dialog.hpp"
#include "validation.hpp"
#include <QFileDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QShortcut>
#include <QTextStream>
#include <memory>
#include <qmessagebox.h>
#include <qtablewidget.h>
#include "util/log.hpp"

MainWindow::MainWindow(const QString& filePath, QWidget *parent)
	: QMainWindow(parent)
	, ui(MainWindowUI())
	, transactionModel()
	, filePath(filePath)
	, transactionFilterWindow(nullptr)
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
	connect(filterShortcut, &QShortcut::activated, this, &MainWindow::toggleFilterWindow);
	connect(ui.filterButton, &QPushButton::clicked, this, &MainWindow::toggleFilterWindow);

	if (!QFile::exists(filePath))
	{
		QMessageBox::warning(this, "Warning", QString("Failed to find file \"%1\". Creating new file.").arg(filePath));
		QFile file(filePath);
		QDir dir;
		bool success = dir.mkpath(QFileInfo(filePath).absolutePath());
		CG_ASSERT(success, "Failed to create directories!");
		success = file.open(QIODevice::ReadWrite | QIODevice::Text);
		CG_ASSERT(success, "Failed to open file!");
		file.close();
	}

	if (!loadFromFile(filePath, transactionModel)) CG_THROW("Failed to load transactions file!");
	if (!transactionModel.isEmpty())
	{
		transactionModel.getFilter().dateMax = transactionModel.getUnfilteredTransactions().at(0)->date.addDays(28);
		transactionModel.getFilter().dateMin = transactionModel.getUnfilteredTransactions().back()->date;
	}
	ui.tableView->setModel(&transactionModel);
	ui.tableView->resizeColumnsToContents();
	if (ui.tableView->columnWidth(4) > 800) ui.tableView->setColumnWidth(4, 800);

	ui.update(transactionModel);
}

MainWindow::~MainWindow()
{}

void MainWindow::openAddTransactionDialog()
{
	TransactionDialog dialog(transactionModel, this);

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
	TransactionGroupDialog dialog(transactionModel, this);

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
		TransactionGroupDialog dialog(transactionModel, *transactionGroup, this);
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
		TransactionDialog dialog(transactionModel, *transaction, this);
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

void MainWindow::toggleFilterWindow()
{
	if (!transactionFilterWindow)
	{
		transactionFilterWindow = new TransactionFilterWindow(transactionModel, this);
		transactionFilterWindow->setWindowFlag(Qt::Window);
		connect(transactionFilterWindow, &TransactionFilterWindow::updateMainUI, this, [this](){ ui.update(transactionModel); });
	}
	if (transactionFilterWindow->isHidden())
	{
		transactionFilterWindow->show();
		transactionModel.setFilterActive(true);
	}
	else
	{
		transactionFilterWindow->hide();
		transactionModel.setFilterActive(false);
	}
}

void MainWindow::saveTransactions()
{
	if (!saveToFile(filePath, transactionModel)) QMessageBox::warning(this, "Error", "Failed to save data!");
}
