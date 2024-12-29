#include "transaction.hpp"
#include <cstdint>
#include <qobject.h>
#include <QDate>
#include <qregularexpression.h>
#include <qvalidator.h>
#include "util/log.hpp"

int32_t getTransactionCategoryIndex(const QString& category)
{
	for (int32_t i = 0; i < transactionCategories.size(); i++)
	{
		if (transactionCategories[i] == category) return i;
	}
	return -1;
}

bool validateTransactionCategory(const QString& name)
{
	for (const QString& category : transactionCategories)
	{
		if (category == name) return true;
	}
	return false;
}

Amount::Amount(int32_t value) : value(value)
{}

Amount::Amount(QString stringValue)
{
	stringValue.remove(QRegularExpression(" "));
	stringValue.remove(QRegularExpression("€"));
	QRegularExpressionValidator validator(QRegularExpression(R"((\+|-)?\b\d+(\.\d{1,2})?\b)"));
	int pos = 0;
	CG_ASSERT(validator.validate(stringValue, pos), "Invalid amount format!");
	bool negative = true;
	if (stringValue.startsWith("+")) negative = false;
	stringValue.remove(QRegularExpression(R"(^(\+|-))"));
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
	if (negative) value = -value;
}

QString Amount::toString() const
{
	return (isNegative() ? "- " : "+ ") + QString::number(std::abs(value) / 100) + "." + QString::number(std::abs(value) % 100).rightJustified(2, '0');
}

bool Amount::isNegative() const
{
	return value < 0;
}

QStringList Transaction::getFieldNames()
{
	namespace tfn = TransactionFieldNames;
	return QStringList({
		tfn::ID,
		tfn::Date,
		tfn::Category,
		tfn::Amount,
		tfn::Description});
}

QString Transaction::getField(const QString& fieldName) const
{
	namespace tfn = TransactionFieldNames;
	if (fieldName == tfn::ID) return QString::number(id);
	else if (fieldName == tfn::Date) return date.toString("dd.MM.yyyy");
	else if (fieldName == tfn::Category) return category;
	else if (fieldName == tfn::Amount) return amount.toString();
	else if (fieldName == tfn::Description) return description;
	else CG_THROW("Invalid transaction field name!");
}

QString Transaction::getFieldView(const QString& fieldName) const
{
	namespace tfn = TransactionFieldNames;
	QString output = getField(fieldName);
	if (fieldName == tfn::Amount) output += " €";
	return output;
}

void Transaction::setField(const QString& fieldName, const QString& value)
{
	namespace tfn = TransactionFieldNames;
	if (fieldName == tfn::ID) id = value.toULongLong();
	else if (fieldName == tfn::Date) date = QDate::fromString(value, "dd.MM.yyyy");
	else if (fieldName == tfn::Category) category = value;
	else if (fieldName == tfn::Amount) amount = Amount(value);
	else if (fieldName == tfn::Description) description = value;
	else CG_THROW("Invalid transaction field name!");
}

QString Transaction::toString() const
{
	QString output = "";
	for (const QString& field : getFieldNames())
	{
		output += field + ": " + getField(field) + "; ";
	}
	return output;
}

bool operator<(const Transaction& a, const Transaction& b)
{
	if (a.date == b.date) return a.id < b.id;
	return a.date < b.date;
}
