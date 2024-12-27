#pragma once

#include <cstdint>
#include <qobject.h>
#include <QDate>

const QStringList categories {
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

inline bool validateTransactionCategory(const QString& name)
{
	for (const QString& category : categories)
	{
		if (category == name) return true;
	}
	return false;
}

struct Amount
{
	// amount in cents
	int32_t value;

	QString toString() const
	{
		return QString::number(value / 100) + "." + QString::number(value % 100).leftJustified(2, '0') + " €";
	}
};

struct Transaction
{
	size_t id;
	QString category;
	QDate date;
	Amount amount;
	QString description;
	size_t reference_id = 0;

	QString toString() const
	{
		return "Date: " + date.toString() + "; Category: " + category + "; Amount: " + amount.toString() + "; Description: " + description + "; Reference ID: " + QString::number(reference_id);
	}

	size_t hash() const
	{
		return std::hash<QString>()(toString());
	}
};
