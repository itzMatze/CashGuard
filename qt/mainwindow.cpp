#include "mainwindow.h"
#include "transaction_model.hpp"
#include "ui_mainwindow.h"
#include "transactiondialog.h"
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
	, transactionModel(filePath)
{
	ui->setupUi(this);

	QShortcut* addShortcut = new QShortcut(QKeySequence("A"), this);
	connect(addShortcut, &QShortcut::activated, this, &MainWindow::openAddTransactionDialog);
	connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::openAddTransactionDialog);
	QShortcut* editShortcut = new QShortcut(QKeySequence("E"), this);
	connect(editShortcut, &QShortcut::activated, this, &MainWindow::openEditTransactionDialog);
	connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::openEditTransactionDialog);
	QShortcut* removeShortcut = new QShortcut(QKeySequence("D"), this);
	connect(removeShortcut, &QShortcut::activated, this, &MainWindow::openDeleteTransactionDialog);
	connect(ui->removeButton, &QPushButton::clicked, this, &MainWindow::openDeleteTransactionDialog);
	QShortcut* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
	connect(saveShortcut, &QShortcut::activated, this, &MainWindow::saveToFile);
	connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveToFile);
	ui->saveButton->setDisabled(true);

	if (!transactionModel.loadFromFile()) QMessageBox::warning(this, "Error", "Failed to load data!");
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
	if (transactionModel.isDirty())
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", "Save changes?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			if (!transactionModel.saveToFile()) QMessageBox::warning(this, "Error", "Failed to save data!");
		}
	}
	delete ui;
}

void MainWindow::openAddTransactionDialog()
{
	TransactionDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		transactionModel.add(dialog.getTransaction());
		ui->saveButton->setDisabled(false);
	}
}

void MainWindow::openEditTransactionDialog()
{
	int32_t idx = ui->tableView->selectionModel()->currentIndex().row();
	TransactionDialog dialog(transactionModel.getTransaction(idx), this);

	if (dialog.exec() == QDialog::Accepted)
	{
		transactionModel.setTransaction(idx, dialog.getTransaction());
		ui->saveButton->setDisabled(false);
	}
}

void MainWindow::openDeleteTransactionDialog()
{
	int32_t idx = ui->tableView->selectionModel()->currentIndex().row();
	QString message(QString("Delete transaction %1?").arg(transactionModel.getTransaction(idx).getField(TransactionFieldNames::ID)));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		transactionModel.removeTransaction(idx);
		ui->saveButton->setDisabled(false);
	}
}

void MainWindow::saveToFile()
{
	if (!transactionModel.isDirty()) return;
	if (!transactionModel.saveToFile()) QMessageBox::warning(this, "Error", "Failed to save data!");
	else ui->saveButton->setDisabled(true);
}
