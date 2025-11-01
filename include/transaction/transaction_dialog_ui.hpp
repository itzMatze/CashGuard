#pragma once

class Transaction;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QPushButton;
class QWidget;

class TransactionDialogUI
{
public:
	TransactionDialogUI(QWidget* parent);
	void update(const Transaction& transaction);

	QDateEdit* date_input;
	QLineEdit* description_input;
	QLineEdit* amount_input;
	QComboBox* category_input;
	QPushButton* ok_button;
	QPushButton* cancel_button;
};
