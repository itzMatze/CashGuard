#include "transaction/transaction_group_dialog_ui.hpp"
#include "total_amount.hpp"
#include "transaction_model.hpp"
#include "qt_util.hpp"
#include "transaction_table_style_delegate.hpp"
#include <QComboBox>
#include <QDateEdit>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

TransactionGroupDialogUI::TransactionGroupDialogUI(QWidget* parent)
	: total_amount_label(new QLabel(parent))
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
	QVBoxLayout* root_layout = new QVBoxLayout(parent);
	total_amount_label->setFont(set_font_size(20, true, total_amount_label->font()));
	root_layout->addWidget(total_amount_label);
	root_layout->addWidget(date_input);
	root_layout->addWidget(category_input);
	root_layout->addWidget(description_input);
	root_layout->addWidget(table_view);
	table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	table_view->setSelectionMode(QAbstractItemView::SingleSelection);
	table_view->setItemDelegate(new TransactionTableStyleDelegate(table_view));
	table_view->horizontalHeader()->setHighlightSections(false);
	table_view->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
	table_view->verticalHeader()->setHighlightSections(false);
	table_view->verticalHeader()->setFocusPolicy(Qt::NoFocus);

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
	parent->setLayout(root_layout);
}

void TransactionGroupDialogUI::update(const TransactionModel& transaction_model)
{
	Amount total_amount = get_filtered_total_amount(transaction_model);
	total_amount_label->setText(total_amount.to_string());
}
