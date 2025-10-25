#pragma once

#include "total_amount.hpp"
#include "transaction_model.hpp"
#include "qt_util.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtableview.h>
#include <qwidget.h>

class TransactionGroupDialogUI
{
public:
	explicit TransactionGroupDialogUI(QWidget* parent)
		: root_layout(new QVBoxLayout(parent))
		, total_amount_label(new QLabel(parent))
		, date_input(new QDateEdit(parent))
		, category_input(new QComboBox(parent))
		, description_input(new QLineEdit(parent))
		, table_view(new QTableView(parent))
		, add_button(new QPushButton(parent))
		, edit_button(new QPushButton(parent))
		, remove_button(new QPushButton(parent))
		, ok_button(new QPushButton(parent))
		, cancel_button(new QPushButton(parent))
	{
		total_amount_label->setFont(set_font_size(20, true, total_amount_label->font()));
		root_layout->addWidget(total_amount_label);
		root_layout->addWidget(date_input);
		root_layout->addWidget(category_input);
		root_layout->addWidget(description_input);
		root_layout->addWidget(table_view);

		add_button->setFont(set_font_size(12, false, add_button->font()));
		add_button->setText("Add");
		edit_button->setFont(set_font_size(12, false, edit_button->font()));
		edit_button->setText("Edit");
		remove_button->setFont(set_font_size(12, false, remove_button->font()));
		remove_button->setText("Remove");
		QHBoxLayout* transaction_button_layout = new QHBoxLayout;
		transaction_button_layout->addWidget(add_button);
		transaction_button_layout->addWidget(edit_button);
		transaction_button_layout->addWidget(remove_button);
		root_layout->addLayout(transaction_button_layout);

		ok_button->setFont(set_font_size(12, false, ok_button->font()));
		ok_button->setText("OK");
		cancel_button->setFont(set_font_size(12, false, cancel_button->font()));
		cancel_button->setText("Cancel");
		QHBoxLayout* confirmation_button_layout = new QHBoxLayout;
		confirmation_button_layout->addWidget(ok_button);
		confirmation_button_layout->addWidget(cancel_button);
		root_layout->addLayout(confirmation_button_layout);
	}

	void update(const TransactionModel& transaction_model)
	{
		Amount total_amount = get_filtered_total_amount(transaction_model);
		total_amount_label->setText(total_amount.to_string());
	}

	QVBoxLayout* root_layout;
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
