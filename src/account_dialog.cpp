#include "account_dialog.hpp"
#include "account_dialog_ui.hpp"
#include <qmessagebox.h>
#include <qshortcut.h>

AccountDialog::AccountDialog(AccountModel& account_model, const Amount& total_amount, QWidget* parent)
	: QDialog(parent)
	, ui(account_model, total_amount, this)
	, account_model(account_model)
	, total_amount(total_amount)
{
	this->setLayout(ui.root_layout);
	this->setGeometry(QRect(QPoint(0, 0), QPoint(1400, 800)));
	QShortcut* add_shortcut = new QShortcut(QKeySequence("Ctrl+A"), this);
	connect(add_shortcut, &QShortcut::activated, this, &AccountDialog::add_account);
	connect(ui.add_button, &QPushButton::clicked, this, &AccountDialog::add_account);
	QShortcut* remove_shortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	connect(remove_shortcut, &QShortcut::activated, this, &AccountDialog::delete_account);
	connect(ui.remove_button, &QPushButton::clicked, this, &AccountDialog::delete_account);
	connect(ui.quit_button, &QPushButton::clicked, this, &QDialog::close);
	connect(&account_model, &QAbstractTableModel::dataChanged, this, &AccountDialog::update);

	ui.table_view->setModel(&account_model);
	ui.table_view->resizeColumnsToContents();
	ui.table_view->resizeRowsToContents();
	ui.table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.table_view->setSelectionMode(QAbstractItemView::SingleSelection);
}

AccountDialog::~AccountDialog()
{}

void AccountDialog::add_account()
{
	account_model.add(Account());
}

void AccountDialog::delete_account()
{
	int32_t idx = ui.table_view->selectionModel()->currentIndex().row();
	if (idx < 0 || idx >= account_model.get_data().size()) return;
	QString message(QString("Delete account %1?").arg(account_model.get_data()[idx].name));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		account_model.remove_account(idx);
	}
}

void AccountDialog::update()
{
	ui.update(account_model, total_amount);
}
