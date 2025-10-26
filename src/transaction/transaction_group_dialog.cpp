#include "transaction/transaction_group_dialog.hpp"
#include "table_style_delegate.hpp"
#include "total_amount.hpp"
#include "transaction.hpp"
#include "transaction/transaction_dialog.hpp"
#include "util/random_generator.hpp"
#include "validation.hpp"
#include <QComboBox>
#include <QCompleter>
#include <QDateEdit>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QTableView>

TransactionGroupDialog::TransactionGroupDialog(const TransactionModel& global_transaction_model, QWidget* parent)
	: QDialog(parent)
	, ui(TransactionGroupDialogUI(this))
	, transaction_model(this)
	, global_transaction_model(global_transaction_model)
{
	transaction_group.id = 0;
	transaction_group.date = QDate::currentDate();
	transaction_group.category = "";
	transaction_group.amount = 0;
	init();
}

TransactionGroupDialog::TransactionGroupDialog(const TransactionModel& global_transaction_model, const TransactionGroup& transaction_group, QWidget* parent)
	: QDialog(parent)
	, ui(TransactionGroupDialogUI(this))
	, transaction_model(this)
	, global_transaction_model(global_transaction_model)
	, transaction_group(transaction_group)
{
	init();
}

TransactionGroupDialog::~TransactionGroupDialog()
{}

void TransactionGroupDialog::init()
{
	QCompleter* completer = new QCompleter(global_transaction_model.get_unique_value_list(TransactionFieldNames::Description), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	ui.description_input->setCompleter(completer);
	transaction_model.set_categories(global_transaction_model.get_category_names(), global_transaction_model.get_category_colors());
	this->setGeometry(QRect(QPoint(0, 0), QPoint(1400, 800)));
	ui.date_input->setDisplayFormat("dd.MM.yyyy");
	ui.date_input->setDate(transaction_group.date);

	ui.category_input->addItems(global_transaction_model.get_category_names());
	ui.category_input->setCurrentText(transaction_group.category);

	ui.description_input->setPlaceholderText("Enter description...");
	ui.description_input->setText(transaction_group.get_field(TransactionFieldNames::Description));

	QShortcut* add_shortcut = new QShortcut(QKeySequence("Ctrl+N"), this);
	connect(add_shortcut, &QShortcut::activated, this, &TransactionGroupDialog::open_add_transaction_dialog);
	connect(ui.add_button, &QPushButton::clicked, this, &TransactionGroupDialog::open_add_transaction_dialog);
	QShortcut* edit_shortcut = new QShortcut(QKeySequence("Ctrl+E"), this);
	connect(edit_shortcut, &QShortcut::activated, this, &TransactionGroupDialog::open_edit_transaction_dialog);
	connect(ui.edit_button, &QPushButton::clicked, this, &TransactionGroupDialog::open_edit_transaction_dialog);
	QShortcut* remove_shortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	connect(remove_shortcut, &QShortcut::activated, this, &TransactionGroupDialog::open_delete_transaction_dialog);
	connect(ui.remove_button, &QPushButton::clicked, this, &TransactionGroupDialog::open_delete_transaction_dialog);
	QShortcut* ok_shortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(ok_shortcut, &QShortcut::activated, this, &QDialog::accept);
	connect(ui.ok_button, &QPushButton::clicked, this, &QDialog::accept);
	QShortcut* cancel_shortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
	connect(cancel_shortcut, &QShortcut::activated, this, &QDialog::reject);
	connect(ui.cancel_button, &QPushButton::clicked, this, &QDialog::reject);

	for (const std::shared_ptr<Transaction>& transaction : transaction_group.transactions) transaction_model.add(transaction);
	ui.table_view->setModel(&transaction_model);
	ui.table_view->resizeColumnsToContents();
	ui.table_view->resizeRowsToContents();
	if (ui.table_view->columnWidth(4) > 800) ui.table_view->setColumnWidth(4, 800);
	ui.table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.table_view->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.table_view->setItemDelegate(new TableStyleDelegate(ui.table_view));
	ui.update(transaction_model);
}

TransactionGroup TransactionGroupDialog::get_transaction_group()
{
	transaction_group.transactions.clear();
	transaction_group.date = ui.date_input->date();
	transaction_group.category = ui.category_input->currentText();
	transaction_group.description = ui.description_input->text();
	// prevent overwriting of the id when a transaction is edited
	if (transaction_group.id == 0) transaction_group.id = rng::random_int64();
	for (uint32_t i = 0; i < transaction_model.rowCount(); i++) transaction_group.transactions.push_back(transaction_model.get_transaction(i));
	transaction_group.amount = get_filtered_total_amount(transaction_model);
	return transaction_group;
}

void TransactionGroupDialog::open_add_transaction_dialog()
{
	Transaction transaction;
	transaction.date = ui.date_input->date();
	transaction.category = ui.category_input->currentText();
	TransactionDialog dialog(global_transaction_model, transaction, this);

	if (dialog.exec() == QDialog::Accepted)
	{
		std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(dialog.get_transaction());
		transaction->added = QDateTime::currentDateTime();
		transaction->edited = QDateTime::currentDateTime();
		transaction_model.add(transaction);
		ui.update(transaction_model);
	}
}

void TransactionGroupDialog::open_edit_transaction_dialog()
{
	int32_t idx = ui.table_view->selectionModel()->currentIndex().row();
	if (!validate_transaction_index(idx, transaction_model)) return;
	std::shared_ptr<Transaction> transaction = transaction_model.get_transaction(idx);
	TransactionDialog dialog(global_transaction_model, *transaction, this);

	if (dialog.exec() == QDialog::Accepted)
	{
		std::shared_ptr<Transaction> new_transaction = std::make_shared<Transaction>(dialog.get_transaction());
		if (*transaction == *new_transaction) return;
		new_transaction->edited = QDateTime::currentDateTime();
		transaction_model.set_transaction(idx, new_transaction);
		ui.update(transaction_model);
	}
}

void TransactionGroupDialog::open_delete_transaction_dialog()
{
	int32_t idx = ui.table_view->selectionModel()->currentIndex().row();
	if (!validate_transaction_index(idx, transaction_model)) return;
	QString message(QString("Delete transaction %1?").arg(transaction_model.get_transaction(idx)->get_field(TransactionFieldNames::ID)));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		transaction_model.remove_transaction(idx);
		ui.update(transaction_model);
	}
}
