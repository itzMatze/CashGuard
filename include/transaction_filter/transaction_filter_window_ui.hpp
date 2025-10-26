#pragma once

class TransactionModel;
class TransactionFilter;
class QCheckBox;
class QComboBox;
class QDateEdit;
class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;

class TransactionFilterWindowUI
{
public:
	TransactionFilterWindowUI(const TransactionModel& global_transaction_model, QWidget* parent);
	void update(const TransactionFilter& transactionFilter);

	QCheckBox* negated_check_box;
	QLabel* date_min_label;
	QDateEdit* date_min_input;
	QLabel* date_max_label;
	QDateEdit* date_max_input;
	QLabel* category_label;
	QComboBox* category_lnput;
	QLabel* amount_min_label;
	QLineEdit* amount_min_input;
	QLabel* amount_max_label;
	QLineEdit* amount_max_input;
	QLabel* description_label;
	QLineEdit* description_input;
	QPushButton* ok_button;
	QPushButton* reset_button;
};
