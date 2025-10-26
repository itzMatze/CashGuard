#include "transaction/transaction_dialog_ui.hpp"
#include "transaction.hpp"
#include <QComboBox>
#include <QDateEdit>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

TransactionDialogUI::TransactionDialogUI(QWidget* parent)
	: date_input(new QDateEdit(parent))
	, description_input(new QLineEdit(parent))
	, amount_input(new QLineEdit(parent))
	, category_input(new QComboBox(parent))
	, ok_button(new QPushButton("OK", parent))
	, cancel_button(new QPushButton("Cancel", parent))
{
	date_input->setDisplayFormat("dd.MM.yyyy");
	description_input->setPlaceholderText("Enter description...");
	amount_input->setPlaceholderText("Enter amount...");

	QHBoxLayout* button_layout = new QHBoxLayout;
	button_layout->addWidget(ok_button);
	button_layout->addWidget(cancel_button);

	QVBoxLayout* root_layout = new QVBoxLayout(parent);
	root_layout->addWidget(date_input);
	root_layout->addWidget(description_input);
	root_layout->addWidget(amount_input);
	root_layout->addWidget(category_input);
	root_layout->addLayout(button_layout);
	parent->setLayout(root_layout);
}

void TransactionDialogUI::update(const Transaction& transaction)
{
	date_input->setDate(transaction.date);
	description_input->setText(transaction.get_field(TransactionFieldNames::Description));
	amount_input->setText(transaction.get_field(TransactionFieldNames::Amount));
	category_input->setCurrentText(transaction.category);
}
