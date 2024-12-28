#pragma once

#include <cstdint>
#include <qobject.h>
#include <QDate>

const QStringList transactionCategories {
	"Cash",
	"Cellular",
	"Clothing",
	"Groceries",
	"Gaming",
	"Leisure",
	"Present",
	"Restaurant",
	"Salary",
	"Saving",
	"Public Transport",
	"Vacation",
	"None"
};

int32_t getTransactionCategoryIndex(const QString& category);
bool validateTransactionCategory(const QString& name);

struct Amount
{
	Amount() = default;
	Amount(int32_t value);
	Amount(QString stringValue);
	QString toString() const;

	// amount in cents
	int32_t value;
};

namespace TransactionFieldNames
{
	const QString ID = "ID";
	const QString Date = "Date";
	const QString Category = "Category";
	const QString Amount = "Amount";
	const QString Description = "Description";
}

class Transaction
{
public:
	static QStringList getFieldNames();
	QString getField(const QString& fieldName) const;
	QString getFieldView(const QString& fieldName) const;
	void setField(const QString& fieldName, const QString& value);
	QString toString() const;

	size_t id;
	QDate date;
	QString category;
	Amount amount;
	QString description;
};
