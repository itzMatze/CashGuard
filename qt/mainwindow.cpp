#include "mainwindow.h"
#include "table_style_delegate.hpp"
#include "total_amount.hpp"
#include "transaction_filter_dialog.h"
#include "transaction_model.hpp"
#include "transaction_file_handler.hpp"
#include "ui_mainwindow.h"
#include "transaction_dialog.h"
#include "transaction_group_dialog.h"
#include <QFileDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QShortcut>
#include <QTextStream>
#include <qtablewidget.h>

MainWindow::MainWindow(const QString& filePath, QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, transactionModel()
	, filePath(filePath)
{
	ui->setupUi(this);

	QShortcut* addShortcut = new QShortcut(QKeySequence("Ctrl+A"), this);
	connect(addShortcut, &QShortcut::activated, this, &MainWindow::openAddTransactionDialog);
	connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::openAddTransactionDialog);
	QShortcut* addGroupShortcut = new QShortcut(QKeySequence("Ctrl+G"), this);
	connect(addGroupShortcut, &QShortcut::activated, this, &MainWindow::openAddTransactionGroupDialog);
	connect(ui->addGroupButton, &QPushButton::clicked, this, &MainWindow::openAddTransactionGroupDialog);
	QShortcut* editShortcut = new QShortcut(QKeySequence("Ctrl+E"), this);
	connect(editShortcut, &QShortcut::activated, this, &MainWindow::openEditTransactionDialog);
	connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::openEditTransactionDialog);
	QShortcut* removeShortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	connect(removeShortcut, &QShortcut::activated, this, &MainWindow::openDeleteTransactionDialog);
	connect(ui->removeButton, &QPushButton::clicked, this, &MainWindow::openDeleteTransactionDialog);
	QShortcut* filterShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
	connect(filterShortcut, &QShortcut::activated, this, &MainWindow::openFilterDialog);

	if (!loadFromFile(filePath, transactionModel)) QMessageBox::warning(this, "Error", "Failed to load data!");
	if (!loadFromFile(filePath, oldTransactionModel)) QMessageBox::warning(this, "Error", "Failed to load data!");
	ui->tableView->setModel(&transactionModel);
	ui->tableView->resizeColumnsToContents();
	ui->tableView->resizeRowsToContents();
	if (ui->tableView->columnWidth(4) > 800) ui->tableView->setColumnWidth(4, 800);
	ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableView->setItemDelegate(new TableStyleDelegate(ui->tableView));

	ui->totalAmountChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	ui->totalAmountChart->setRenderHint(QPainter::Antialiasing);
	updateWindow();
}

MainWindow::~MainWindow()
{
	if (!saveToFile(filePath + ".old", oldTransactionModel)) QMessageBox::warning(this, "Error", "Failed to save backup data!");
	delete ui;
}

void MainWindow::openAddTransactionDialog()
{
	TransactionDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		transactionModel.add(std::make_shared<Transaction>(dialog.getTransaction()));
		updateWindow();
		saveTransactions();
	}
}

void MainWindow::openAddTransactionGroupDialog()
{
	TransactionGroupDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		transactionModel.add(std::make_shared<TransactionGroup>(dialog.getTransactionGroup()));
		updateWindow();
		saveTransactions();
	}
}

void MainWindow::openEditTransactionDialog()
{
	int32_t idx = ui->tableView->selectionModel()->currentIndex().row();
	std::shared_ptr<const Transaction> transaction = transactionModel.getTransaction(idx);
	if (std::shared_ptr<const TransactionGroup> transaction_group = std::dynamic_pointer_cast<const TransactionGroup>(transaction))
	{
		TransactionGroupDialog dialog(*transaction_group, this);
		if (dialog.exec() == QDialog::Accepted)
		{
			transactionModel.setTransaction(idx, std::make_shared<TransactionGroup>(dialog.getTransactionGroup()));
		}
	}
	else
	{
		TransactionDialog dialog(*transaction, this);
		if (dialog.exec() == QDialog::Accepted)
		{
			transactionModel.setTransaction(idx, std::make_shared<Transaction>(dialog.getTransaction()));
		}
	}
	updateWindow();
	saveTransactions();
}

void MainWindow::openDeleteTransactionDialog()
{
	int32_t idx = ui->tableView->selectionModel()->currentIndex().row();
	QString message(QString("Delete transaction %1?").arg(transactionModel.getTransaction(idx)->getField(TransactionFieldNames::ID)));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		transactionModel.removeTransaction(idx);
		updateWindow();
		saveTransactions();
	}
}

void MainWindow::openFilterDialog()
{
	TransactionFilterDialog dialog(transactionModel.getFilter(), this);

	if (dialog.exec() == QDialog::Accepted)
	{
		transactionModel.setFilter(dialog.getTransactionFilter());
		updateWindow();
	}
}

void MainWindow::saveTransactions()
{
	if (!saveToFile(filePath, transactionModel)) QMessageBox::warning(this, "Error", "Failed to save data!");
}

void MainWindow::updateWindow()
{
	ui->totalAmountLabel->setText(getCurrentTotalAmount(transactionModel).toString() + " €");
	ui->totalAmountChart->setChart(getSmallTotalAmountChart(transactionModel));
}
