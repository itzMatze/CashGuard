#pragma once

#include "transaction/transaction_dialog_ui.hpp"
#include "transaction.hpp"
#include <QDialog>

class TransactionModel;

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
	TransactionDialogUI ui;
	const TransactionModel& global_transaction_model;
	Transaction transaction;

	void init();
	void auto_complete_from_description();
};
