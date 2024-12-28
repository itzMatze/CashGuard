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
	Q_OBJECT

public:
	explicit MainWindow(const QString& filePath, QWidget *parent = nullptr);
	~MainWindow();

private slots:
  void openAddTransactionDialog();
  void openEditTransactionDialog();
  void openDeleteTransactionDialog();
	void saveToFile();

private:
	Ui::MainWindow *ui;
	TransactionModel transactionModel;
};

#endif // MAINWINDOW_H
