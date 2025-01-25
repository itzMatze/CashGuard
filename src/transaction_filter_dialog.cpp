#include "transaction_filter_dialog.hpp"
#include <limits>
#include <qshortcut.h>

TransactionFilterDialog::TransactionFilterDialog(const TransactionModel& transactionModel, QWidget *parent) : transactionModel(transactionModel), transactionFilter(transactionModel.getFilter())
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

	dateMaxLabel = new QLabel(this);
	dateMaxLabel->setText("Max Date");
	dateMaxInput = new QDateEdit(this);
	dateMaxInput->setDisplayFormat("dd.MM.yyyy");

	categoryLabel = new QLabel(this);
	categoryLabel->setText("Category (None to disable filtering)");
	categoryInput = new QComboBox(this);
	categoryInput->addItems(transactionCategories);

	amountMinLabel = new QLabel(this);
	amountMinLabel->setText("Min Amount");
	amountMinInput = new QLineEdit(this);
	amountMinInput->setPlaceholderText("Enter amount...");

	amountMaxLabel = new QLabel(this);
	amountMaxLabel->setText("Max Amount");
	amountMaxInput = new QLineEdit(this);
	amountMaxInput->setPlaceholderText("Enter amount...");

	descriptionLabel = new QLabel(this);
	descriptionLabel->setText("Search Phrase");
	descriptionInput = new QLineEdit(this);
	descriptionInput->setPlaceholderText("Enter phrase...");

	QPushButton* okButton = new QPushButton("OK", this);
	QPushButton* cancelButton = new QPushButton("Cancel", this);
	QPushButton* resetButton = new QPushButton("Reset", this);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addWidget(resetButton);

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
	QShortcut* resetShortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
	connect(resetShortcut, &QShortcut::activated, this, &TransactionFilterDialog::resetFilter);
	connect(resetButton, &QPushButton::clicked, this, &TransactionFilterDialog::resetFilter);

	updateWindow();
}

void TransactionFilterDialog::updateWindow()
{
	dateMinInput->setDate(transactionFilter.dateMin);
	dateMaxInput->setDate(transactionFilter.dateMax);
	categoryInput->setCurrentIndex(getTransactionCategoryIndex(transactionFilter.category));
	if (transactionFilter.amountMin.value != std::numeric_limits<int32_t>::min()) amountMinInput->setText(transactionFilter.amountMin.toString());
	if (transactionFilter.amountMax.value != std::numeric_limits<int32_t>::max()) amountMaxInput->setText(transactionFilter.amountMax.toString());
	descriptionInput->setText(transactionFilter.searchPhrase);
}

TransactionFilter TransactionFilterDialog::getTransactionFilter()
{
	transactionFilter.dateMin = dateMinInput->date();
	transactionFilter.dateMax = dateMaxInput->date();
	transactionFilter.category = categoryInput->currentText();
	transactionFilter.amountMin = (amountMinInput->text().size() > 0) ? Amount{amountMinInput->text()} : Amount(std::numeric_limits<int32_t>::min());
	transactionFilter.amountMax = (amountMaxInput->text().size() > 0) ? Amount{amountMaxInput->text()} : Amount(std::numeric_limits<int32_t>::max());
	transactionFilter.searchPhrase = descriptionInput->text();
	return transactionFilter;
}

void TransactionFilterDialog::resetFilter()
{
	transactionFilter = TransactionFilter();
	transactionFilter.dateMax = transactionModel.getUnfilteredTransactions().at(0)->date;
	transactionFilter.dateMin = transactionModel.getUnfilteredTransactions().back()->date;
	updateWindow();
}
