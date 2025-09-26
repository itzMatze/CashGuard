#pragma once

#include "account_model.hpp"
#include "account_dialog_ui.hpp"
#include <QDialog>

namespace Ui {
class AccountDialog;
}

class AccountDialog : public QDialog
{
	Q_OBJECT;

public:
	explicit AccountDialog(AccountModel& accountModel, const Amount& totalAmount, QWidget *parent = nullptr);
	~AccountDialog();

private slots:
	void addAccount();
	void deleteAccount();
	void update();

private:
	AccountDialogUI ui;
	AccountModel& accountModel;
	const Amount& totalAmount;
};
