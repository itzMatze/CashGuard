#include "transaction_filter_dialog.hpp"
#include <limits>
#include <qcompleter.h>
#include <qshortcut.h>

TransactionFilterDialog::TransactionFilterDialog(const TransactionModel& globalTransactionModel, QWidget *parent) : globalTransactionModel(globalTransactionModel), transactionFilter(globalTransactionModel.getFilter())
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
	categoryInput->addItems(globalTransactionModel.getCategoryNames());

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

	filterActiveCheckBox = new QCheckBox(this);
	filterActiveCheckBox->setText("Filter Active");

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
	mainLayout->addWidget(filterActiveCheckBox);
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

	QShortcut* activateShortcut = new QShortcut(QKeySequence("Ctrl+T"), this);
	connect(activateShortcut, &QShortcut::activated, this, [this]() { filterActiveCheckBox->toggle(); });

	updateWindow();
}

void TransactionFilterDialog::updateWindow()
{
	filterActiveCheckBox->setChecked(transactionFilter.active);
	dateMinInput->setDate(transactionFilter.dateMin);
	dateMaxInput->setDate(transactionFilter.dateMax);
	categoryInput->setCurrentText(transactionFilter.category);
	if (transactionFilter.amountMin.value != std::numeric_limits<int32_t>::min()) amountMinInput->setText(transactionFilter.amountMin.toString());
	else amountMinInput->setText("");
	if (transactionFilter.amountMax.value != std::numeric_limits<int32_t>::max()) amountMaxInput->setText(transactionFilter.amountMax.toString());
	else amountMaxInput->setText("");
	descriptionInput->setText(transactionFilter.searchPhrase);
}

bool TransactionFilterDialog::validateInputs()
{
	if (dateMinInput->date() > dateMaxInput->date()) return false;
	if (Amount{amountMinInput->text()} > Amount{amountMaxInput->text()}) return false;
	return true;
}

TransactionFilter TransactionFilterDialog::getTransactionFilter()
{
	transactionFilter.active = filterActiveCheckBox->isChecked();
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
	transactionFilter.dateMax = globalTransactionModel.getUnfilteredTransactions().at(0)->date;
	transactionFilter.dateMin = globalTransactionModel.getUnfilteredTransactions().back()->date;
	updateWindow();
}

void TransactionFilterDialog::accept()
{
	if (validateInputs()) QDialog::accept();
	else QMessageBox::warning(this, "Invalid Input", "Please correct the inputs.");
}

void TransactionFilterDialog::setRecommender(const QStringList& recommendations)
{
	QCompleter* completer = new QCompleter(recommendations, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	descriptionInput->setCompleter(completer);
}
