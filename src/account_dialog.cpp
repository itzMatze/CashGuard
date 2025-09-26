#include "account_dialog.hpp"
#include "account_dialog_ui.hpp"
#include <qmessagebox.h>
#include <qshortcut.h>

AccountDialog::AccountDialog(AccountModel& accountModel, const Amount& totalAmount, QWidget *parent)
	: QDialog(parent)
	, ui(accountModel, totalAmount)
	, accountModel(accountModel)
	, totalAmount(totalAmount)
{
	this->setLayout(ui.rootLayout);
	this->setGeometry(QRect(QPoint(0, 0), QPoint(1400, 800)));
	QShortcut* addShortcut = new QShortcut(QKeySequence("Ctrl+A"), this);
	connect(addShortcut, &QShortcut::activated, this, &AccountDialog::addAccount);
	connect(ui.addButton, &QPushButton::clicked, this, &AccountDialog::addAccount);
	QShortcut* removeShortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	connect(removeShortcut, &QShortcut::activated, this, &AccountDialog::deleteAccount);
	connect(ui.removeButton, &QPushButton::clicked, this, &AccountDialog::deleteAccount);
	connect(ui.quitButton, &QPushButton::clicked, this, &QDialog::close);
	connect(&accountModel, &QAbstractTableModel::dataChanged, this, &AccountDialog::update);

	ui.tableView->setModel(&accountModel);
	ui.tableView->resizeColumnsToContents();
	ui.tableView->resizeRowsToContents();
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
}

AccountDialog::~AccountDialog()
{}

void AccountDialog::addAccount()
{
	accountModel.add(Account());
}

void AccountDialog::deleteAccount()
{
	int32_t idx = ui.tableView->selectionModel()->currentIndex().row();
	if (idx < 0 || idx >= accountModel.getData().size()) return;
	QString message(QString("Delete account %1?").arg(accountModel.getData()[idx].name));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		accountModel.removeAccount(idx);
	}
}

void AccountDialog::update()
{
	ui.update(accountModel, totalAmount);
}
