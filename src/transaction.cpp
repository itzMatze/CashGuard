#include "transaction.hpp"
#include <cstdint>
#include <qcolor.h>
#include <qobject.h>
#include <QDate>
#include <qregularexpression.h>
#include <qvalidator.h>
#include "util/log.hpp"

Amount::Amount(int32_t value) : value(value)
{}

Amount::Amount(QString string_value)
{
	string_value.remove(QRegularExpression(" "));
	string_value.remove(QRegularExpression("€"));
	QRegularExpressionValidator validator(QRegularExpression(R"((\+|-)?\b\d+(\.\d{1,2})?\b)"));
	int pos = 0;
	CG_ASSERT(validator.validate(string_value, pos), "Invalid amount format!");
	bool negative = true;
	if (string_value.startsWith("+")) negative = false;
	string_value.remove(QRegularExpression(R"(^(\+|-))"));
	if (!string_value.contains('.'))
	{
		value = 100 * string_value.toInt();
	}
	else
	{
		QStringList string_values = string_value.split('.');
		int32_t euro = string_values.at(0).toInt();
		int32_t cent = string_values.at(1).leftJustified(2, '0').toInt();
		value = cent + 100 * euro;
	}
	if (negative) value = -value;
}

QString Amount::to_string() const
{
	return (is_negative() ? "-" : "+") + QString::number(std::abs(value) / 100) + "." + QString::number(std::abs(value) % 100).rightJustified(2, '0');
}

bool Amount::is_negative() const
{
	return value < 0;
}

bool operator<(const Amount& a, const Amount& b)
{
	return a.value < b.value;
}

bool operator>(const Amount& a, const Amount& b)
{
	return a.value > b.value;
}

Transaction::Transaction() :
	id(0),
	date(QDate::currentDate()),
	amount(0),
	description(""),
	added(QDateTime::currentDateTime()),
	edited(QDateTime::currentDateTime())
{}

QStringList Transaction::get_field_names()
{
	namespace tfn = TransactionFieldNames;
	return QStringList({
		tfn::ID,
		tfn::Date,
		tfn::Category,
		tfn::Amount,
		tfn::Description,
		tfn::Added,
		tfn::Edited});
}

QString Transaction::get_field(const QString& field_name) const
{
	namespace tfn = TransactionFieldNames;
	if (field_name == tfn::ID) return QString::number(id);
	else if (field_name == tfn::Date) return date.toString("dd.MM.yyyy");
	else if (field_name == tfn::Category) return category;
	else if (field_name == tfn::Amount) return amount.to_string();
	else if (field_name == tfn::Description) return description;
	else if (field_name == tfn::Added) return added.toString("dd.MM.yyyy HH:mm:ss");
	else if (field_name == tfn::Edited) return edited.toString("dd.MM.yyyy HH:mm:ss");
	else CG_THROW("Invalid transaction field name!");
}

QString Transaction::get_field_view(const QString& field_name) const
{
	namespace tfn = TransactionFieldNames;
	QString output = get_field(field_name);
	if (field_name == tfn::Amount) output += " €";
	return output;
}

void Transaction::set_field(const QString& field_name, const QString& value)
{
	namespace tfn = TransactionFieldNames;
	if (field_name == tfn::ID) id = value.toULongLong();
	else if (field_name == tfn::Date) date = QDate::fromString(value, "dd.MM.yyyy");
	else if (field_name == tfn::Category) category = value;
	else if (field_name == tfn::Amount) amount = Amount(value);
	else if (field_name == tfn::Description) description = value;
	else if (field_name == tfn::Added) added = QDateTime::fromString(value, "dd.MM.yyyy HH:mm:ss");
	else if (field_name == tfn::Edited) edited = QDateTime::fromString(value, "dd.MM.yyyy HH:mm:ss");
	else CG_THROW("Invalid transaction field name!");
}

QString Transaction::to_string() const
{
	QString output = "";
	for (const QString& field : get_field_names())
	{
		output += field + ": " + get_field(field) + "; ";
	}
	return output;
}

bool operator<(const Transaction& a, const Transaction& b)
{
	if (a.date == b.date)
	{
		if (a.added == b.added) return a.id < b.id;
		else return a.added < b.added;
	}
	else return a.date < b.date;
}

bool operator==(const Transaction& a, const Transaction& b)
{
	QStringList fields = Transaction::get_field_names();
	for (const QString& field : fields)
	{
		if (a.get_field(field) != b.get_field(field)) return false;
	}
	return true;
}

TransactionGroup::TransactionGroup(const Transaction& transaction) : Transaction(transaction)
{}

QString TransactionGroup::to_string() const
{
	return Transaction::to_string() + "Transaction Group";
}

bool operator==(const TransactionGroup& a, const TransactionGroup& b)
{
	QStringList fields = Transaction::get_field_names();
	for (const QString& field : fields)
	{
		if (a.get_field(field) != b.get_field(field)) return false;
	}
	if (a.transactions.size() != b.transactions.size()) return false;
	for (int32_t i = 0; i < a.transactions.size(); i++)
	{
		if (*a.transactions[i] != *b.transactions[i]) return false;
	}
	return true;
}
