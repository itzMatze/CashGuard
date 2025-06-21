#ifndef TRANSACTIONDIALOG_H
#define TRANSACTIONDIALOG_H

#include "transaction.hpp"
#include "transaction_model.hpp"
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
	TransactionDialog(const TransactionModel& globalTransactionModel, QWidget *parent = nullptr);
	TransactionDialog(const TransactionModel& globalTransactionModel, const Transaction& transaction, QWidget *parent = nullptr);
	Transaction getTransaction();

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

private:
	QDateEdit* dateInput;
	QComboBox* categoryInput;
	QLineEdit* amountInput;
	QLineEdit* descriptionInput;
	const TransactionModel& globalTransactionModel;
	Transaction transaction;

	void init();
};

#endif // TRANSACTIONDIALOG_H
