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
#include <qtablewidget.h>

[[nodiscard]] bool loadData(const QString& fileName, std::vector<Transaction>& transactions)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
	transactions.clear();
	QTextStream in(&file);
	// skip header
	QStringList fields = in.readLine().split(',');
	while (!in.atEnd())
	{
		QStringList cells = in.readLine().split(',');
		Transaction transaction;
		for (uint32_t i = 0; i < fields.size(); i++)
		{
			transaction.setField(fields.at(i), cells.at(i));
		}
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

	QStringList fields = Transaction::getFieldNames();
	QString outLine = "";
	for (const QString& field : fields) outLine += field + ",";
	outLine.removeLast();
	out << outLine << "\n";
	for (const Transaction& t : transactions)
	{
		outLine = "";
		for (uint32_t i = 0; i < fields.size(); i++) outLine += t.getField(fields.at(i)) + ",";
		outLine.removeLast();
		out << outLine << "\n";
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

	if (dialog.exec() == QDialog::Accepted)
	{
		transactions.push_back(dialog.getTransaction());
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
	std::sort(transactions.begin(), transactions.end(), [](const Transaction& a, const Transaction& b){ return a.date < b.date; });
	ui->tableWidget->clear();
	ui->tableWidget->setRowCount(transactions.size());
	QStringList fields = Transaction::getFieldNames();
	ui->tableWidget->setColumnCount(fields.size());
	ui->tableWidget->setHorizontalHeaderLabels(fields);

	for (uint32_t i = 0; i < transactions.size(); i++)
	{
		for (uint32_t j = 0; j < fields.size(); j++) ui->tableWidget->setItem(i, j, new QTableWidgetItem(transactions[i].getField(fields.at(j))));
	}
	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
	if (ui->tableWidget->columnWidth(4) > 800) ui->tableWidget->setColumnWidth(4, 800);
}
