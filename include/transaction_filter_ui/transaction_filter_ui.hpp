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
	TransactionFilterUI(const TransactionModel& global_transaction_model, QWidget* parent)
		: negated_check_box(new QCheckBox(parent))
		, date_min_label(new QLabel(parent))
		, date_min_input(new QDateEdit(parent))
		, date_max_label(new QLabel(parent))
		, date_max_input(new QDateEdit(parent))
		, category_label(new QLabel(parent))
		, category_lnput(new QComboBox(parent))
		, amount_min_label(new QLabel(parent))
		, amount_min_input(new QLineEdit(parent))
		, amount_max_label(new QLabel(parent))
		, amount_max_input(new QLineEdit(parent))
		, description_label(new QLabel(parent))
		, description_input(new QLineEdit(parent))
		, ok_button(new QPushButton(parent))
		, reset_button(new QPushButton(parent))
	{
		QVBoxLayout* root_layout = new QVBoxLayout(parent);
		negated_check_box->setText("Negate Filter");
		root_layout->addWidget(negated_check_box);

		QVBoxLayout* date_layout = new QVBoxLayout;
		QHBoxLayout* date_label_layout = new QHBoxLayout;
		date_min_label->setText("Min Date");
		date_label_layout->addWidget(date_min_label);
		date_max_label->setText("Max Date");
		date_label_layout->addWidget(date_max_label);
		date_layout->addLayout(date_label_layout);
		QHBoxLayout* date_input_layout = new QHBoxLayout;
		date_min_input->setDisplayFormat("dd.MM.yyyy");
		date_input_layout->addWidget(date_min_input);
		date_max_input->setDisplayFormat("dd.MM.yyyy");
		date_input_layout->addWidget(date_max_input);
		date_layout->addLayout(date_input_layout);
		date_layout->setAlignment(Qt::AlignmentFlag::AlignVCenter);
		root_layout->addLayout(date_layout);

		QVBoxLayout* category_layout = new QVBoxLayout;
		category_label->setText("Category (None to disable filtering)");
		category_layout->addWidget(category_label);
		category_lnput->addItems(global_transaction_model.get_category_names());
		category_layout->addWidget(category_lnput);
		category_layout->setAlignment(Qt::AlignmentFlag::AlignVCenter);
		root_layout->addLayout(category_layout);

		QVBoxLayout* amount_layout = new QVBoxLayout;
		QHBoxLayout* amount_label_layout = new QHBoxLayout;
		amount_min_label->setText("Min Amount");
		amount_label_layout->addWidget(amount_min_label);
		amount_max_label->setText("Max Amount");
		amount_label_layout->addWidget(amount_max_label);
		amount_layout->addLayout(amount_label_layout);
		QHBoxLayout* amount_input_layout = new QHBoxLayout;
		amount_min_input->setPlaceholderText("Enter amount...");
		amount_input_layout->addWidget(amount_min_input);
		amount_max_input->setPlaceholderText("Enter amount...");
		amount_input_layout->addWidget(amount_max_input);
		amount_layout->addLayout(amount_input_layout);
		amount_layout->setAlignment(Qt::AlignmentFlag::AlignVCenter);
		root_layout->addLayout(amount_layout);

		QVBoxLayout* description_layout = new QVBoxLayout;
		description_label->setText("Search Phrase");
		description_layout->addWidget(description_label);
		description_input->setPlaceholderText("Enter phrase...");
		description_layout->addWidget(description_input);
		description_layout->setAlignment(Qt::AlignmentFlag::AlignVCenter);
		root_layout->addLayout(description_layout);

		QHBoxLayout* button_layout = new QHBoxLayout;
		ok_button->setText("Apply");
		button_layout->addWidget(ok_button);
		reset_button->setText("Reset");
		button_layout->addWidget(reset_button);
		root_layout->addLayout(button_layout);

		parent->setLayout(root_layout);
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
