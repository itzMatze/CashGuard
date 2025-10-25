#pragma once

#include "account_model.hpp"
#include "qt_util.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtableview.h>
#include <qwidget.h>

class AccountDialogUI
{
public:
	explicit AccountDialogUI(const AccountModel& account_model, const Amount& total_amount, QWidget* parent)
		: root_layout(new QVBoxLayout(parent))
		, table_view(new QTableView(parent))
		, add_button(new QPushButton(parent))
		, remove_button(new QPushButton(parent))
		, quit_button(new QPushButton(parent))
		, match_label(new QLabel(parent))
	{
		add_button->setFont(set_font_size(12, false, add_button->font()));
		add_button->setText("Add");
		remove_button->setFont(set_font_size(12, false, remove_button->font()));
		remove_button->setText("Remove");
		quit_button->setFont(set_font_size(12, false, quit_button->font()));
		quit_button->setText("Quit");
		QHBoxLayout* button_layout = new QHBoxLayout;
		button_layout->addWidget(add_button);
		button_layout->addWidget(remove_button);
		button_layout->addWidget(quit_button);
		match_label->setFont(set_font_size(20, true, match_label->font()));
		root_layout->addWidget(match_label);
		root_layout->addWidget(table_view);
		root_layout->addLayout(button_layout);
		update(account_model, total_amount);
	}

	void update(const AccountModel& account_model, const Amount& total_amount)
	{
		Amount account_total_amount = account_model.get_total_amount();
		if (account_total_amount.value == total_amount.value)
		{
			match_label->setText("Amounts match!");
			match_label->setStyleSheet("QLabel { color: #00ff00; }");
		}
		else
		{
			match_label->setText("Amounts don't match! Accounts: " + account_total_amount.to_string() + " €, Difference: " + Amount(account_total_amount.value - total_amount.value).to_string() + " €");
			match_label->setStyleSheet("QLabel { color: #ff0000; }");
		}
	}

	QVBoxLayout* root_layout;
	QTableView* table_view;
	QPushButton* add_button;
	QPushButton* remove_button;
	QPushButton* quit_button;
	QLabel* match_label;
};
