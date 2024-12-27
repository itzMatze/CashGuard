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
	Q_OBJECT

public:
	TransactionDialog(QWidget *parent = nullptr);
	Transaction getTransaction() const;

private:
	QDateEdit* dateInput;
	QComboBox* categoryInput;
	QLineEdit* amountInput;
	QLineEdit* descriptionInput;
};

#endif // TRANSACTIONDIALOG_H
