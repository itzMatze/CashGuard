#pragma once

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
	TransactionDialog(const TransactionModel& global_transaction_model, QWidget* parent);
	TransactionDialog(const TransactionModel& global_transaction_model, const Transaction& transaction, QWidget* parent);
	Transaction get_transaction();

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

private:
	QDateEdit* date_input;
	QComboBox* category_input;
	QLineEdit* amount_input;
	QLineEdit* description_input;
	const TransactionModel& global_transaction_model;
	Transaction transaction;

	void init();
	void auto_complete_from_description();
};
