#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_ui.hpp"
#include "transaction_filter_ui/transaction_filter_window.hpp"
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
  void openAddTransactionGroupDialog();
  void openEditTransactionDialog();
  void openDeleteTransactionDialog();
  void toggleFilterWindow();
	void saveTransactions();

private:
	MainWindowUI ui;
	TransactionModel transactionModel;
	QString filePath;
	TransactionFilterWindow* transactionFilterWindow;
};

#endif // MAINWINDOW_H
