#ifndef TRANSACTIONDIALOG_H
#define TRANSACTIONDIALOG_H

#include "transaction.hpp"
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qlineedit.h>

class TransactionDialog : public QDialog
{
	Q_OBJECT;

public:
	TransactionDialog(QWidget *parent = nullptr);
	TransactionDialog(const Transaction& transaction, QWidget *parent = nullptr);
	Transaction getTransaction();

private:
	QDateEdit* dateInput;
	QComboBox* categoryInput;
	QLineEdit* amountInput;
	QLineEdit* descriptionInput;
	Transaction transaction;

	void init();
};

#endif // TRANSACTIONDIALOG_H
