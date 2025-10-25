#pragma once

#include "transaction_filter.hpp"
#include "transaction_model.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qwidget.h>

class TransactionFilterUI
{
public:
	TransactionFilterUI(const TransactionModel& global_transaction_model, QWidget* parent) :
		central_widget(new QWidget(parent)),
		negated_check_box(new QCheckBox(parent)),
		date_min_label(new QLabel(parent)),
		date_min_input(new QDateEdit(parent)),
		date_max_label(new QLabel(parent)),
		date_max_input(new QDateEdit(parent)),
		category_label(new QLabel(parent)),
		category_lnput(new QComboBox(parent)),
		amount_min_label(new QLabel(parent)),
		amount_min_input(new QLineEdit(parent)),
		amount_max_label(new QLabel(parent)),
		amount_max_input(new QLineEdit(parent)),
		description_label(new QLabel(parent)),
		description_input(new QLineEdit(parent)),
		ok_button(new QPushButton(parent)),
		reset_button(new QPushButton(parent))
	{
		date_min_label->setText("Min Date");
		date_min_input->setDisplayFormat("dd.MM.yyyy");
		date_max_label->setText("Max Date");
		date_max_input->setDisplayFormat("dd.MM.yyyy");
		category_label->setText("Category (None to disable filtering)");
		category_lnput->addItems(global_transaction_model.get_category_names());
		amount_min_label->setText("Min Amount");
		amount_min_input->setPlaceholderText("Enter amount...");
		amount_max_label->setText("Max Amount");
		amount_max_input->setPlaceholderText("Enter amount...");
		description_label->setText("Search Phrase");
		description_input->setPlaceholderText("Enter phrase...");
		negated_check_box->setText("Negate Filter");

		ok_button->setText("OK");
		reset_button->setText("Reset");
		QHBoxLayout* button_layout = new QHBoxLayout(parent);
		button_layout->addWidget(ok_button);
		button_layout->addWidget(reset_button);

		QHBoxLayout* date_label_layout = new QHBoxLayout(parent);
		date_label_layout->addWidget(date_min_label);
		date_label_layout->addWidget(date_max_label);
		QHBoxLayout* date_layout = new QHBoxLayout(parent);
		date_layout->addWidget(date_min_input);
		date_layout->addWidget(date_max_input);

		QHBoxLayout* amount_label_layout = new QHBoxLayout(parent);
		amount_label_layout->addWidget(amount_min_label);
		amount_label_layout->addWidget(amount_max_label);
		QHBoxLayout* amount_layout = new QHBoxLayout(parent);
		amount_layout->addWidget(amount_min_input);
		amount_layout->addWidget(amount_max_input);

		QVBoxLayout* root_layout = new QVBoxLayout(parent);
		root_layout->addWidget(negated_check_box);
		root_layout->addLayout(date_label_layout);
		root_layout->addLayout(date_layout);
		root_layout->addWidget(category_label);
		root_layout->addWidget(category_lnput);
		root_layout->addLayout(amount_label_layout);
		root_layout->addLayout(amount_layout);
		root_layout->addWidget(description_label);
		root_layout->addWidget(description_input);
		root_layout->addLayout(button_layout);
		central_widget->setLayout(root_layout);
	}

	void update(const TransactionFilter& transactionFilter)
	{
		date_min_input->setDate(transactionFilter.date_min);
		date_max_input->setDate(transactionFilter.date_max);
		category_lnput->setCurrentText(transactionFilter.category);
		if (transactionFilter.amount_min.value != std::numeric_limits<int32_t>::min()) amount_min_input->setText(transactionFilter.amount_min.to_string());
		else amount_min_input->setText("");
		if (transactionFilter.amount_max.value != std::numeric_limits<int32_t>::max()) amount_max_input->setText(transactionFilter.amount_max.to_string());
		else amount_max_input->setText("");
		description_input->setText(transactionFilter.search_phrase);
	}

	QWidget* central_widget;
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

private:
};
