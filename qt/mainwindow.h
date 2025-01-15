#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "transaction_model.hpp"
#include <QMainWindow>
#include <qshortcut.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT;

public:
	explicit MainWindow(const QString& filePath, QWidget *parent = nullptr);
	~MainWindow();

private slots:
  void openAddTransactionDialog();
  void openEditTransactionDialog();
  void openDeleteTransactionDialog();
	void saveTransactions();

private:
	Ui::MainWindow *ui;
	TransactionModel transactionModel;
	TransactionModel oldTransactionModel;
	QString filePath;
};

#endif // MAINWINDOW_H
