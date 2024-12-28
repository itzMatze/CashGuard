#pragma once

#include <cstdint>
#include <qobject.h>
#include <QDate>
#include <qregularexpression.h>
#include <qvalidator.h>
#include "util/log.hpp"
#include "util/random_generator.hpp"

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
	Amount() = default;
	Amount(int32_t value) : value(value)
	{}

	Amount(QString stringValue)
	{
		stringValue.remove(QRegularExpression(" €"));
		QRegularExpressionValidator validator(QRegularExpression(R"(\b\d+(\.\d{1,2})?\b)"));
		int pos = 0;
		CG_ASSERT(validator.validate(stringValue, pos), "Invalid amount format!");
		if (!stringValue.contains('.'))
		{
			value = 100 * stringValue.toInt();
		}
		else
		{
			QStringList stringValues = stringValue.split('.');
			int32_t euro = stringValues.at(0).toInt();
			int32_t cent = stringValues.at(1).toInt();
			value = cent + 100 * euro;
		}
	}

	// amount in cents
	int32_t value;

	QString toString() const
	{
		return QString::number(value / 100) + "." + QString::number(value % 100).leftJustified(2, '0') + " €";
	}
};

namespace TransactionFieldNames
{
	const QString ID = "ID";
	const QString Date = "Date";
	const QString Category = "Category";
	const QString Amount = "Amount";
	const QString Description = "Description";
	const QString Reference_ID = "Reference_ID";
}

class Transaction
{
public:
	static QStringList getFieldNames()
	{
		namespace tfn = TransactionFieldNames;
		return QStringList({
			tfn::ID,
			tfn::Date,
			tfn::Category,
			tfn::Amount,
			tfn::Description,
			tfn::Reference_ID});
	}

	QString getField(const QString& fieldName) const
	{
		namespace tfn = TransactionFieldNames;
		if (fieldName == tfn::ID) return QString::number(id);
		else if (fieldName == tfn::Date) return date.toString("dd.MM.yyyy");
		else if (fieldName == tfn::Category) return category;
		else if (fieldName == tfn::Amount) return amount.toString();
		else if (fieldName == tfn::Description) return description;
		else if (fieldName == tfn::Reference_ID) return QString::number(reference_id);
		else CG_THROW("Invalid transaction field name!");
	}

	void setField(const QString& fieldName, const QString& value)
	{
		namespace tfn = TransactionFieldNames;
		if (fieldName == tfn::ID) id = value.toULongLong();
		else if (fieldName == tfn::Date) date = QDate::fromString(value, "dd.MM.yyyy");
		else if (fieldName == tfn::Category) category = value;
		else if (fieldName == tfn::Amount) amount = Amount(value);
		else if (fieldName == tfn::Description) description = value;
		else if (fieldName == tfn::Reference_ID) reference_id = value.toULongLong();
		else CG_THROW("Invalid transaction field name!");
	}

	QString toString() const
	{
		QString output = "";
		for (const QString& field : getFieldNames())
		{
			output += field + ": " + getField(field) + "; ";
		}
		return output;
	}

	size_t hash() const
	{
		namespace tfn = TransactionFieldNames;
		return std::hash<QString>()(
			getField(tfn::Date) +
			getField(tfn::Category) +
			getField(tfn::Amount) +
			getField(tfn::Description) +
			QString::number(rng::random_int32()));
	}

	size_t id;
	QDate date;
	QString category;
	Amount amount;
	QString description;
	size_t reference_id = 0;
};
