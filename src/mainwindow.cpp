#include "mainwindow.hpp"
#include "mainwindow_ui.hpp"
#include "account_dialog.hpp"
#include "total_amount.hpp"
#include "transaction.hpp"
#include "transaction_filter/transaction_filter_window.hpp"
#include "transaction_model.hpp"
#include "transaction_file_handler.hpp"
#include "transaction/transaction_dialog.hpp"
#include "transaction/transaction_group_dialog.hpp"
#include "validation.hpp"
#include <QFileDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QShortcut>
#include <QTextStream>
#include <memory>
#include <qmessagebox.h>
#include <qtablewidget.h>
#include "util/log.hpp"

MainWindow::MainWindow()
	: QMainWindow(nullptr)
	, ui(this)
	, transaction_model(this)
	, account_model(this)
	, transaction_filter_window(nullptr)
{
	setWindowTitle("Cash Guard");
	setCentralWidget(ui.central_widget);
	QShortcut* add_shortcut = new QShortcut(QKeySequence("Ctrl+N"), this);
	connect(add_shortcut, &QShortcut::activated, this, &MainWindow::open_add_transaction_dialog);
	connect(ui.add_button, &QPushButton::clicked, this, &MainWindow::open_add_transaction_dialog);
	QShortcut* add_group_shortcut = new QShortcut(QKeySequence("Ctrl+G"), this);
	connect(add_group_shortcut, &QShortcut::activated, this, &MainWindow::open_add_transaction_group_dialog);
	connect(ui.add_group_button, &QPushButton::clicked, this, &MainWindow::open_add_transaction_group_dialog);
	QShortcut* edit_shortcut = new QShortcut(QKeySequence("Ctrl+E"), this);
	connect(edit_shortcut, &QShortcut::activated, this, &MainWindow::open_edit_transaction_dialog);
	connect(ui.edit_button, &QPushButton::clicked, this, &MainWindow::open_edit_transaction_dialog);
	QShortcut* remove_shortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	connect(remove_shortcut, &QShortcut::activated, this, &MainWindow::open_delete_transaction_dialog);
	connect(ui.remove_button, &QPushButton::clicked, this, &MainWindow::open_delete_transaction_dialog);
	QShortcut* filter_shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
	connect(filter_shortcut, &QShortcut::activated, this, &MainWindow::toggle_filter_window);
	connect(ui.filter_button, &QPushButton::clicked, this, &MainWindow::toggle_filter_window);
	QShortcut* account_shortcut = new QShortcut(QKeySequence("Ctrl+A"), this);
	connect(account_shortcut, &QShortcut::activated, this, &MainWindow::open_accounts_dialog);
	connect(ui.account_button, &QPushButton::clicked, this, &MainWindow::open_accounts_dialog);
}

bool MainWindow::init(const QString& file_path)
{
	this->file_path = file_path;
	if (!QFile::exists(file_path))
	{
		QMessageBox::warning(this, "Warning", QString("Failed to find file \"%1\". Creating new file.").arg(file_path));
		QFile file(file_path);
		QDir dir;
		bool success = dir.mkpath(QFileInfo(file_path).absolutePath());
		CG_ASSERT(success, "Failed to create directories!");
		success = file.open(QIODevice::ReadWrite | QIODevice::Text);
		CG_ASSERT(success, "Failed to open file!");
		file.close();
	}

	if (!load_from_file(file_path, transaction_model, account_model))
	{
		QMessageBox::warning(this, "Error", QString("Failed to load file \"%1\". Exiting.").arg(file_path));
		return false;
	}
	if (!transaction_model.is_empty())
	{
		transaction_model.get_filter().date_max = transaction_model.get_unfiltered_transactions().at(0)->date.addDays(28);
		transaction_model.get_filter().date_min = transaction_model.get_unfiltered_transactions().back()->date;
	}
	ui.table_view->setModel(&transaction_model);
	ui.table_view->resizeColumnsToContents();
	if (ui.table_view->columnWidth(4) > 800) ui.table_view->setColumnWidth(4, 800);

	update();
	return true;
}

MainWindow::~MainWindow()
{}

void MainWindow::update()
{
	filtered_total_amount = get_filtered_total_amount(transaction_model);
	global_total_amount = get_global_total_amount(transaction_model);
	ui.update(transaction_model, account_model, filtered_total_amount, global_total_amount);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (account_model.get_total_amount().value != global_total_amount.value)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(
			this,
			"Exit program",
			"Account sum does not match transaction sum. Are you sure you want to exit?",
			QMessageBox::Yes | QMessageBox::No
		);
		if (reply != QMessageBox::Yes)
		{
			event->ignore();
			return;
		}
	}
	event->accept();
}

void MainWindow::open_add_transaction_dialog()
{
	TransactionDialog dialog(transaction_model, this);

	if (dialog.exec() == QDialog::Accepted)
	{
		std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(dialog.get_transaction());
		transaction->added = QDateTime::currentDateTime();
		transaction->edited = QDateTime::currentDateTime();
		transaction_model.add(transaction);
		update();
		save_transactions();
	}
}

void MainWindow::open_add_transaction_group_dialog()
{
	TransactionGroupDialog dialog(transaction_model, this);

	if (dialog.exec() == QDialog::Accepted)
	{
		std::shared_ptr<TransactionGroup> transaction = std::make_shared<TransactionGroup>(dialog.get_transaction_group());
		transaction->added = QDateTime::currentDateTime();
		transaction->edited = QDateTime::currentDateTime();
		transaction_model.add(transaction);
		update();
		save_transactions();
	}
}

void MainWindow::open_edit_transaction_dialog()
{
	int32_t idx = ui.table_view->selectionModel()->currentIndex().row();
	if (!validate_transaction_index(idx, transaction_model)) return;
	std::shared_ptr<Transaction> transaction = transaction_model.get_transaction(idx);
	if (std::shared_ptr<TransactionGroup> transaction_group = std::dynamic_pointer_cast<TransactionGroup>(transaction))
	{
		TransactionGroupDialog dialog(transaction_model, *transaction_group, this);
		if (dialog.exec() == QDialog::Accepted)
		{
			std::shared_ptr<TransactionGroup> new_transaction_group = std::make_shared<TransactionGroup>(dialog.get_transaction_group());
			if (*transaction_group == *new_transaction_group) return;
			new_transaction_group->edited = QDateTime::currentDateTime();
			transaction_model.set_transaction(idx, new_transaction_group);
		}
	}
	else
	{
		TransactionDialog dialog(transaction_model, *transaction, this);
		if (dialog.exec() == QDialog::Accepted)
		{
			std::shared_ptr<Transaction> new_transaction = std::make_shared<Transaction>(dialog.get_transaction());
			if (*transaction == *new_transaction) return;
			new_transaction->edited = QDateTime::currentDateTime();
			transaction_model.set_transaction(idx, new_transaction);
		}
	}
	update();
	save_transactions();
}

void MainWindow::open_delete_transaction_dialog()
{
	int32_t idx = ui.table_view->selectionModel()->currentIndex().row();
	if (!validate_transaction_index(idx, transaction_model)) return;
	QString message(QString("Delete transaction %1?").arg(transaction_model.get_transaction(idx)->get_field(TransactionFieldNames::ID)));
	QMessageBox::StandardButton reply = QMessageBox::question(this, "CashGuard", message, QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		transaction_model.remove_transaction(idx);
		update();
		save_transactions();
	}
}

void MainWindow::toggle_filter_window()
{
	if (!transaction_filter_window)
	{
		transaction_filter_window = new TransactionFilterWindow(transaction_model, this);
		transaction_filter_window->setWindowFlag(Qt::Window);
		connect(transaction_filter_window, &TransactionFilterWindow::update_main_ui, this, [this](){ update(); });
	}
	if (transaction_filter_window->isHidden())
	{
		transaction_filter_window->show();
		transaction_model.set_filter_active(true);
	}
	else
	{
		transaction_filter_window->hide();
		transaction_model.set_filter_active(false);
	}
}

void MainWindow::open_accounts_dialog()
{
	AccountDialog account_dialog(account_model, global_total_amount, this);
	account_dialog.exec();
	update();
	save_transactions();
}

void MainWindow::save_transactions()
{
	if (!save_to_file(file_path, transaction_model, account_model)) QMessageBox::warning(this, "Error", "Failed to save data!");
}
