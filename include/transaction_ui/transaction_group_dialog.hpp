#ifndef TRANSACTION_GROUP_DIALOG_H
#define TRANSACTION_GROUP_DIALOG_H

#include "transaction.hpp"
#include "transaction_model.hpp"
#include "transaction_group_dialog_ui.hpp"
#include <QDialog>

namespace Ui {
class TransactionGroupDialog;
}

class TransactionGroupDialog : public QDialog
{
	Q_OBJECT;

public:
	explicit TransactionGroupDialog(const TransactionModel& transactionModel, QWidget *parent = nullptr);
	TransactionGroupDialog(const TransactionModel& transactionModel, const TransactionGroup& transactionGroup, QWidget *parent = nullptr);
	~TransactionGroupDialog();
	TransactionGroup getTransactionGroup();

private slots:
	void openAddTransactionDialog();
	void openEditTransactionDialog();
	void openDeleteTransactionDialog();

private:
	TransactionGroupDialogUI ui;
	TransactionModel transactionModel;
	const TransactionModel& globalTransactionModel;
	TransactionGroup transactionGroup;

	void init();
};

#endif // TRANSACTION_GROUP_DIALOG_H
