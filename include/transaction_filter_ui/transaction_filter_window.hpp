#pragma once

#include "transaction_filter_ui/transaction_filter_ui.hpp"
#include "transaction_model.hpp"
#include <qshortcut.h>

class TransactionFilterWindow : public QWidget
{
	Q_OBJECT;

public:
	explicit TransactionFilterWindow(TransactionModel& global_transaction_model, QWidget* parent = nullptr);

private:
	TransactionFilterUI ui;
	TransactionFilter transaction_filter;
	TransactionModel& global_transaction_model;

	void apply();
	void reset();

signals:
	void update_main_ui();
};
