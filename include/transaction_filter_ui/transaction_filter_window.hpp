#pragma once

#include "transaction_filter_ui/transaction_filter_ui.hpp"
#include "transaction_model.hpp"
#include <qshortcut.h>

class TransactionFilterWindow : public QWidget
{
	Q_OBJECT;

public:
	explicit TransactionFilterWindow(TransactionModel& globalTransactionModel, QWidget* parent = nullptr);

private:
	TransactionFilterUI ui;
	TransactionFilter transactionFilter;
	TransactionModel& globalTransactionModel;

	void apply();
	void reset();

signals:
	void updateMainUI();
};
