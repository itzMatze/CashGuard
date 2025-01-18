#include "transaction_filter_dialog.h"
#include <limits>
#include <qshortcut.h>

TransactionFilterDialog::TransactionFilterDialog(QWidget *parent) : QDialog(parent)
{
	init();
}

TransactionFilterDialog::TransactionFilterDialog(const TransactionFilter& transactionFilter, QWidget *parent) : transactionFilter(transactionFilter)
{
	init();
}

void TransactionFilterDialog::init()
{
	setWindowTitle("Transaction Filter Dialog");
	resize(500, 150);

	dateMinLabel = new QLabel(this);
	dateMinLabel->setText("Min Date");
	dateMinInput = new QDateEdit(this);
	dateMinInput->setDisplayFormat("dd.MM.yyyy");
	dateMinInput->setDate(transactionFilter.dateMin);

	dateMaxLabel = new QLabel(this);
	dateMaxLabel->setText("Max Date");
	dateMaxInput = new QDateEdit(this);
	dateMaxInput->setDisplayFormat("dd.MM.yyyy");
	dateMaxInput->setDate(transactionFilter.dateMax);

	categoryLabel = new QLabel(this);
	categoryLabel->setText("Category (None to disable filtering)");
	categoryInput = new QComboBox(this);
	categoryInput->addItems(transactionCategories);
	categoryInput->setCurrentIndex(getTransactionCategoryIndex(transactionFilter.category));

	amountMinLabel = new QLabel(this);
	amountMinLabel->setText("Min Amount");
	amountMinInput = new QLineEdit(this);
	amountMinInput->setPlaceholderText("Enter amount...");
	if (transactionFilter.amountMin.value != std::numeric_limits<int32_t>::min()) amountMinInput->setText(transactionFilter.amountMin.toString());

	amountMaxLabel = new QLabel(this);
	amountMaxLabel->setText("Max Amount");
	amountMaxInput = new QLineEdit(this);
	amountMaxInput->setPlaceholderText("Enter amount...");
	if (transactionFilter.amountMax.value != std::numeric_limits<int32_t>::max()) amountMaxInput->setText(transactionFilter.amountMax.toString());

	descriptionLabel = new QLabel(this);
	descriptionLabel->setText("Search Phrase");
	descriptionInput = new QLineEdit(this);
	descriptionInput->setPlaceholderText("Enter phrase...");
	descriptionInput->setText(transactionFilter.searchPhrase);

	QPushButton* okButton = new QPushButton("OK", this);
	QPushButton* cancelButton = new QPushButton("Cancel", this);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);

	QHBoxLayout* dateLabelLayout = new QHBoxLayout();
	dateLabelLayout->addWidget(dateMinLabel);
	dateLabelLayout->addWidget(dateMaxLabel);
	QHBoxLayout* dateLayout = new QHBoxLayout();
	dateLayout->addWidget(dateMinInput);
	dateLayout->addWidget(dateMaxInput);

	QHBoxLayout* amountLabelLayout = new QHBoxLayout();
	amountLabelLayout->addWidget(amountMinLabel);
	amountLabelLayout->addWidget(amountMaxLabel);
	QHBoxLayout* amountLayout = new QHBoxLayout();
	amountLayout->addWidget(amountMinInput);
	amountLayout->addWidget(amountMaxInput);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(dateLabelLayout);
	mainLayout->addLayout(dateLayout);
	mainLayout->addWidget(categoryLabel);
	mainLayout->addWidget(categoryInput);
	mainLayout->addLayout(amountLabelLayout);
	mainLayout->addLayout(amountLayout);
	mainLayout->addWidget(descriptionLabel);
	mainLayout->addWidget(descriptionInput);
	mainLayout->addLayout(buttonLayout);

	QShortcut* okShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
	connect(okShortcut, &QShortcut::activated, this, &QDialog::accept);
	connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
	QShortcut* cancelShortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
	connect(cancelShortcut, &QShortcut::activated, this, &QDialog::reject);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

TransactionFilter TransactionFilterDialog::getTransactionFilter()
{
	transactionFilter.dateMin = dateMinInput->date();
	transactionFilter.dateMax = dateMaxInput->date();
	transactionFilter.category = categoryInput->currentText();
	transactionFilter.amountMin = Amount{amountMinInput->text()};
	transactionFilter.amountMax = Amount{amountMaxInput->text()};
	transactionFilter.searchPhrase = descriptionInput->text();
	return transactionFilter;
}
