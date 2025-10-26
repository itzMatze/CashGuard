#pragma once

#include "transaction_filter/transaction_filter_window_ui.hpp"
#include "transaction_filter/transaction_filter.hpp"
#include <QWidget>

class TransactionModel;

class TransactionFilterWindow : public QWidget
{
	Q_OBJECT;

public:
	explicit TransactionFilterWindow(TransactionModel& global_transaction_model, QWidget* parent);

private:
	TransactionFilterWindowUI ui;
	TransactionFilter transaction_filter;
	TransactionModel& global_transaction_model;

	void apply();
	void reset();

signals:
	void update_main_ui();
};
