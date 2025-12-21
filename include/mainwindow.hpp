#pragma once

#include "account_model.hpp"
#include "mainwindow_ui.hpp"
#include "transaction_filter/transaction_filter_window.hpp"
#include "transaction_model.hpp"
#include <QMainWindow>
#include <qshortcut.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT;

public:
	MainWindow();
	~MainWindow();
	bool init(const QString& file_path);
	void update();

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	void open_add_transaction_dialog();
	void open_add_transaction_group_dialog();
	void open_edit_transaction_dialog();
	void open_delete_transaction_dialog();
	void toggle_filter_window();
	void open_accounts_dialog();
	void save_transactions();

private:
	MainWindowUI ui;
	TransactionModel transaction_model;
	AccountModel account_model;
	QString file_path;
	TransactionFilterWindow* transaction_filter_window;
	Amount filtered_total_amount;
	Amount global_total_amount;
};
