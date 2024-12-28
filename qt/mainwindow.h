#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "transaction_model.hpp"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
  void openAddTransactionDialog();
	void saveToFile();
	void loadFromFile();

private:
	Ui::MainWindow *ui;
	TransactionModel transactionModel;
};

#endif // MAINWINDOW_H
