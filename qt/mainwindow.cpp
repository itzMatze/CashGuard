#include "mainwindow.h"
#include "transaction.hpp"
#include "ui_mainwindow.h"
#include "transactiondialog.h"
#include <QFile>
#include <QFileDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QTextStream>

[[nodiscard]] bool loadData(const QString& fileName, std::vector<Transaction>& transactions)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
	transactions.clear();
	QTextStream in(&file);
	// skip header
	in.readLine();
	while (!in.atEnd())
	{
		QString line = in.readLine();
		QStringList cells = line.split(',');
		Transaction transaction;
		transaction.id = cells.at(0).toULongLong();
		transaction.date = QDate::fromString(cells.at(1), "dd.MM.yyyy");
		transaction.category = cells.at(2);
		transaction.amount.value = cells.at(3).toInt();
		transaction.description = cells.at(4);
		transactions.push_back(transaction);
	}

	file.close();
	return true;
}

[[nodiscard]] bool saveData(const QString& filePath, const std::vector<Transaction>& transactions)
{
	QFile file(filePath);
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) return false;
	QTextStream out(&file);

	out << "id,date,category,amount,description,reference_id\n";
	for (const Transaction& t : transactions)
	{
		out << t.id << "," << t.date.toString("dd.MM.yyyy") << "," << t.category << "," << t.amount.value << "," << t.description << t.reference_id << "\n";
	}

	file.close();
	return true;
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::openAddTransactionDialog);
	connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveToFile);
	connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadFromFile);

	if (!loadData("transactions.csv", transactions)) QMessageBox::warning(this, "Error", "Failed to load data!");
	updateTable();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::openAddTransactionDialog()
{
	TransactionDialog dialog(this);

	// Show dialog and check if user confirmed
	if (dialog.exec() == QDialog::Accepted)
	{
		Transaction transaction;
		transaction.date = dialog.getDate();
		transaction.category = dialog.getCategory();
		transaction.amount = Amount{dialog.getAmount().toInt()};
		transaction.description = dialog.getDescription();
		transaction.id = transaction.hash();
		transactions.push_back(transaction);
		updateTable();
	}
}

void MainWindow::saveToFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Choose File");
	if (fileName.isEmpty() || !fileName.endsWith(".csv")) return;
	if (!saveData(fileName, transactions)) QMessageBox::warning(this, "Error", "Failed to save data!");
	else QMessageBox::information(this, "Success", "Transactions saved!");
}

void MainWindow::loadFromFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open File");

	if (fileName.isEmpty() || !fileName.endsWith(".csv")) return;
	if (!loadData(fileName, transactions))
	{
		QMessageBox::warning(this, "Error", "Failed to load data!");
		return;
	}
	updateTable();
}

void MainWindow::updateTable()
{
	ui->tableWidget->clear();
	ui->tableWidget->setRowCount(transactions.size());
	ui->tableWidget->setColumnCount(6);
	ui->tableWidget->setHorizontalHeaderLabels({"ID", "Date", "Category", "Amount", "Description", "Reference ID"});

	for (uint32_t i = 0; i < transactions.size(); i++)
	{
		ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(transactions[i].id)));
		ui->tableWidget->setItem(i, 1, new QTableWidgetItem(transactions[i].date.toString("dd.MM.yyyy")));
		ui->tableWidget->setItem(i, 2, new QTableWidgetItem(transactions[i].category));
		ui->tableWidget->setItem(i, 3, new QTableWidgetItem(transactions[i].amount.toString()));
		ui->tableWidget->setItem(i, 4, new QTableWidgetItem(transactions[i].description));
		ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(transactions[i].reference_id)));
	}
	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
	if (ui->tableWidget->columnWidth(4) > 800) ui->tableWidget->setColumnWidth(4, 800);
}
