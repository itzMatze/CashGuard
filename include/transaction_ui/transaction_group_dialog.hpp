#pragma once

#include "transaction.hpp"
#include "transaction_model.hpp"
#include "transaction_group_dialog_ui.hpp"
#include <QDialog>

class TransactionGroupDialog : public QDialog
{
	Q_OBJECT;

public:
	explicit TransactionGroupDialog(const TransactionModel& transaction_model, QWidget* parent);
	TransactionGroupDialog(const TransactionModel& transaction_model, const TransactionGroup& transaction_group, QWidget* parent);
	~TransactionGroupDialog();
	TransactionGroup get_transaction_group();

private slots:
	void open_add_transaction_dialog();
	void open_edit_transaction_dialog();
	void open_delete_transaction_dialog();

private:
	TransactionGroupDialogUI ui;
	TransactionModel transaction_model;
	const TransactionModel& global_transaction_model;
	TransactionGroup transaction_group;

	void init();
};
