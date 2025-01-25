#ifndef TRANSACTIONFILTERDIALOG_H
#define TRANSACTIONFILTERDIALOG_H

#include "transaction_filter.hpp"
#include "transaction_model.hpp"
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qlabel.h>
#include <qlineedit.h>

class TransactionFilterDialog : public QDialog
{
	Q_OBJECT;

public:
	TransactionFilterDialog(const TransactionModel& transactionModel, QWidget *parent);
	TransactionFilter getTransactionFilter();
	void resetFilter();

private:
	QLabel* dateMinLabel;
	QDateEdit* dateMinInput;
	QLabel* dateMaxLabel;
	QDateEdit* dateMaxInput;
	QLabel* categoryLabel;
	QComboBox* categoryInput;
	QLabel* amountMinLabel;
	QLineEdit* amountMinInput;
	QLabel* amountMaxLabel;
	QLineEdit* amountMaxInput;
	QLabel* descriptionLabel;
	QLineEdit* descriptionInput;
	TransactionFilter transactionFilter;
	const TransactionModel& transactionModel;

	void init();
	void updateWindow();
};

#endif // TRANSACTIONFILTERDIALOG_H
