#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transactiondialog.h"
#include <QFileDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QTextStream>
#include <qtablewidget.h>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::openAddTransactionDialog);
	connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::openEditTransactionDialog);
	connect(ui->removeButton, &QPushButton::clicked, this, &MainWindow::openRemoveTransactionDialog);
	connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveToFile);
	connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadFromFile);

	if (!transactionModel.loadFromFile("transactions.csv")) QMessageBox::warning(this, "Error", "Failed to load data!");
	ui->tableView->setModel(&transactionModel);
	ui->tableView->resizeColumnsToContents();
	ui->tableView->resizeRowsToContents();
	if (ui->tableView->columnWidth(4) > 800) ui->tableView->setColumnWidth(4, 800);
	ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableView->setStyleSheet(
    "QTableView::item:hover {"
    "    background-color: transparent;"
    "}"
);
}

MainWindow::~MainWindow()
{
	if (!transactionModel.saveToFile("transactions.csv")) QMessageBox::warning(this, "Error", "Failed to save data!");
	delete ui;
}

void MainWindow::openAddTransactionDialog()
{
	TransactionDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		transactionModel.add(dialog.getTransaction());
	}
}

void MainWindow::openEditTransactionDialog()
{
	int32_t idx = ui->tableView->selectionModel()->currentIndex().row();
	TransactionDialog dialog(transactionModel.getTransaction(idx), this);

	if (dialog.exec() == QDialog::Accepted)
	{
		transactionModel.setTransaction(idx, dialog.getTransaction());
	}
}

void MainWindow::openRemoveTransactionDialog()
{
	int32_t idx = ui->tableView->selectionModel()->currentIndex().row();
	QString message(QString("Remove transaction %1?").arg(transactionModel.getTransaction(idx).getField(TransactionFieldNames::ID)));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) transactionModel.removeTransaction(idx);
}

void MainWindow::saveToFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Choose File");
	if (fileName.isEmpty() || !fileName.endsWith(".csv")) return;
	if (!transactionModel.saveToFile(fileName)) QMessageBox::warning(this, "Error", "Failed to save data!");
	else QMessageBox::information(this, "Success", "Transactions saved!");
}

void MainWindow::loadFromFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open File");

	if (fileName.isEmpty() || !fileName.endsWith(".csv")) return;
	if (!transactionModel.loadFromFile(fileName))
	{
		QMessageBox::warning(this, "Error", "Failed to load data!");
		return;
	}
}
