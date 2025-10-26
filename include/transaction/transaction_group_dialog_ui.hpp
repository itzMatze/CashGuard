#pragma once

class TransactionModel;
class QComboBox;
class QDateEdit;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableView;
class QWidget;

class TransactionGroupDialogUI
{
public:
	explicit TransactionGroupDialogUI(QWidget* parent);
	void update(const TransactionModel& transaction_model);

	QLabel* total_amount_label;
	QDateEdit* date_input;
	QComboBox* category_input;
	QLineEdit* description_input;
	QTableView* table_view;

	QPushButton* add_button;
	QPushButton* edit_button;
	QPushButton* remove_button;

	QPushButton* ok_button;
	QPushButton* cancel_button;
};
