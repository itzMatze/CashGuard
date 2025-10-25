#pragma once

#include "account_model.hpp"
#include "account_dialog_ui.hpp"
#include <QDialog>

class AccountDialog : public QDialog
{
	Q_OBJECT;

public:
	explicit AccountDialog(AccountModel& account_model, const Amount& total_amount, QWidget* parent);
	~AccountDialog();

private slots:
	void add_account();
	void delete_account();
	void update();

private:
	AccountDialogUI ui;
	AccountModel& account_model;
	const Amount& total_amount;
};
