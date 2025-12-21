#include "cg_file_handler.hpp"
#include <set>
#include <QFile>
#include <QMessageBox>
#include "transaction_model.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "util/log.hpp"

const char* version_string = "0.2.0";

Transaction parse_transaction(const auto& rj_transaction)
{
	QStringList fields = Transaction::get_field_names();
	Transaction transaction;
	for (const QString& field : fields)
	{
		if (!rj_transaction.HasMember(field.toStdString().c_str())) continue;
		transaction.set_field(field, rj_transaction[field.toStdString().c_str()].GetString());
	}
	return transaction;
}

bool CGFileHandler::load_from_file(const QString& file_path, TransactionModel& transaction_model, AccountModel& account_model)
{
	cglog::debug("Loading file \"{}\"", file_path.toStdString());
	transaction_model.clear();
	QFile file(file_path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		cglog::error("Failed to open file \"{}\"", file_path.toStdString());
		return false;
	};
	if (file.size() == 0) return true;
	QTextStream in(&file);
	QString file_content = in.readAll();
	hash = qHash(file_content, 0);

	rapidjson::Document doc;
	doc.Parse(file_content.toStdString().c_str());
	if (doc.HasParseError())
	{
		cglog::error("Failed to parse file \"{}\"", file_path.toStdString());
		return false;
	}

	if (std::string(doc["Version"].GetString()) != std::string(version_string))
	{
		cglog::error("Unsupported version in file \"{}\"", file_path.toStdString());
		return false;
	}

	transaction_model.add_category("None", QColor());
	for (const auto& rj_category : doc["Categories"].GetArray()) transaction_model.add_category(rj_category["Name"].GetString(), QColor(rj_category["Color"].GetString()));
	for (const auto& rj_account : doc["Accounts"].GetArray()) account_model.add(Account{.name = rj_account["Name"].GetString(), .amount = Amount(rj_account["Amount"].GetString())});
	std::set<uint64_t> ids;
	for (const auto& rj_transaction : doc["Transactions"].GetArray())
	{
		Transaction transaction = parse_transaction(rj_transaction);
		if (!ids.emplace(transaction.id).second)
		{
			CG_THROW("Duplicate ID: {}", transaction.get_field(TransactionFieldNames::ID).toStdString());
		}
		if (rj_transaction.HasMember("Transactions"))
		{
			std::shared_ptr<TransactionGroup> transaction_group = std::make_shared<TransactionGroup>(transaction);
			for (const auto& rj_sub_transaction : rj_transaction["Transactions"].GetArray())
			{
				transaction_group->transactions.push_back(std::make_shared<Transaction>(parse_transaction(rj_sub_transaction)));
			}
			transaction_model.add(transaction_group);
		}
		else
		{
			transaction_model.add(std::make_shared<Transaction>(transaction));
		}
	}
	return true;
}

void serialize_transaction(const Transaction& transaction, rapidjson::Value& json_object, rapidjson::Document::AllocatorType& allocator)
{
	json_object.SetObject();
	QStringList fields = Transaction::get_field_names();
	for (const QString& field : fields)
	{
		rapidjson::Value name(field.toStdString().c_str(), allocator);
		rapidjson::Value value(transaction.get_field(field).toStdString().c_str(), allocator);
		json_object.AddMember(name, value, allocator);
	}
}

bool CGFileHandler::save_to_file(const QString& file_path, const TransactionModel& transaction_model, const AccountModel& account_model)
{
	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::MemoryPoolAllocator<>& allocator = doc.GetAllocator();
	rapidjson::Value value(version_string, allocator);
	doc.AddMember("Version", value, allocator);

	rapidjson::Value json_categories;
	json_categories.SetArray();
	for (const QString& category : transaction_model.get_category_names())
	{
		if (category == "None") continue;
		rapidjson::Value json_object;
		json_object.SetObject();
		{
			rapidjson::Value value(category.toStdString().c_str(), allocator);
			json_object.AddMember("Name", value, allocator);
		}
		{
			rapidjson::Value value(transaction_model.get_category_colors().at(category).name(QColor::NameFormat::HexArgb).toStdString().c_str(), allocator);
			json_object.AddMember("Color", value, allocator);
		}
		json_categories.PushBack(json_object, allocator);
	}
	doc.AddMember("Categories", json_categories, allocator);

	rapidjson::Value json_accounts;
	json_accounts.SetArray();
	for (const Account& account : account_model.get_data())
	{
		rapidjson::Value json_object;
		json_object.SetObject();
		{
			rapidjson::Value value(account.name.toStdString().c_str(), allocator);
			json_object.AddMember("Name", value, allocator);
		}
		{
			rapidjson::Value value(account.amount.to_string().toStdString().c_str(), allocator);
			json_object.AddMember("Amount", value, allocator);
		}
		json_accounts.PushBack(json_object, allocator);
	}
	doc.AddMember("Accounts", json_accounts, allocator);

	rapidjson::Value json_transactions;
	json_transactions.SetArray();
	for (std::shared_ptr<Transaction> transaction : transaction_model.get_unfiltered_transactions())
	{
		if (std::shared_ptr<const TransactionGroup> transaction_group = std::dynamic_pointer_cast<const TransactionGroup>(transaction))
		{
			rapidjson::Value group;
			serialize_transaction(*transaction, group, allocator);
			rapidjson::Value json_sub_transactions;
			json_sub_transactions.SetArray();
			for (const std::shared_ptr<const Transaction> sub_transaction : transaction_group->transactions)
			{
				rapidjson::Value json_object;
				serialize_transaction(*sub_transaction, json_object, allocator);
				json_sub_transactions.PushBack(json_object, allocator);
			}
			group.AddMember("Transactions", json_sub_transactions, allocator);
			json_transactions.PushBack(group, allocator);
		}
		else
		{
			rapidjson::Value json_object;
			serialize_transaction(*transaction, json_object, allocator);
			json_transactions.PushBack(json_object, allocator);
		}
	}
	doc.AddMember("Transactions", json_transactions, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);

	// check if file hash is still the one we expect
	QFile file(file_path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		cglog::error("Failed to open file \"{}\"", file_path.toStdString());
		return false;
	}
	QTextStream in(&file);
	QString file_content = in.readAll();
	uint32_t new_hash = qHash(file_content, 0);
	if (new_hash != hash)
	{
		cglog::error("Failed to save file! \"{}\" has been modified outside this program!", file_path.toStdString());
		return false;
	}
	file.close();
	// write file
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text))
	{
		cglog::error("Failed to open file \"{}\"", file_path.toStdString());
		return false;
	}
	QTextStream out(&file);
	QString output = QString(buffer.GetString());
	hash = qHash(output);
	out << output;
	file.close();
	return true;
}
