#include "cg_file_handler.hpp"
#include <fstream>
#include <set>
#include "transaction_model.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "util/log.hpp"

const char* version_string = "0.2.0";

Transaction parse_transaction(const auto& rj_transaction)
{
	std::vector<std::string> fields = Transaction::get_field_names();
	Transaction transaction;
	for (const std::string& field : fields)
	{
		if (!rj_transaction.HasMember(field.c_str())) continue;
		transaction.set_field(field, rj_transaction[field.c_str()].GetString());
	}
	return transaction;
}

bool CGFileHandler::load_from_file(const std::string& file_path, TransactionModel& transaction_model, AccountModel& account_model)
{
	cglog::debug("Loading file \"{}\"", file_path);
	transaction_model.clear();
	std::ifstream file(file_path);
	if (!file.is_open())
	{
		cglog::error("Failed to open file \"{}\"", file_path);
		return false;
	}
	std::stringstream file_stream;
	file_stream << file.rdbuf();
	std::string file_content = file_stream.str();
	if (file_content.size() == 0) return true;
	hash = std::hash<std::string>{}(file_content);

	rapidjson::Document doc;
	doc.Parse(file_content.c_str());
	if (doc.HasParseError())
	{
		cglog::error("Failed to parse file \"{}\"", file_path);
		return false;
	}

	if (std::string(doc["Version"].GetString()) != std::string(version_string))
	{
		cglog::error("Unsupported version in file \"{}\"", file_path);
		return false;
	}

	transaction_model.add_category("None", Color());
	for (const auto& rj_category : doc["Categories"].GetArray()) transaction_model.add_category(rj_category["Name"].GetString(), Color(rj_category["Color"].GetString()));
	for (const auto& rj_account : doc["Accounts"].GetArray())
	{
		Account account{.name = rj_account["Name"].GetString()};
		if (!to_amount(rj_account["Amount"].GetString(), account.amount))
		{
			cglog::error("Failed to parse amount!");
			return false;
		}
		account_model.add(account);
	}
	std::set<uint64_t> ids;
	for (const auto& rj_transaction : doc["Transactions"].GetArray())
	{
		Transaction transaction = parse_transaction(rj_transaction);
		if (!ids.emplace(transaction.id).second)
		{
			CG_THROW("Duplicate ID: {}", transaction.get_field(TransactionFieldNames::ID));
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
	std::vector<std::string> fields = Transaction::get_field_names();
	for (const std::string& field : fields)
	{
		rapidjson::Value name(field.c_str(), allocator);
		rapidjson::Value value(transaction.get_field(field).c_str(), allocator);
		json_object.AddMember(name, value, allocator);
	}
}

bool CGFileHandler::save_to_file(const std::string& file_path, const TransactionModel& transaction_model, const AccountModel& account_model)
{
	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::MemoryPoolAllocator<>& allocator = doc.GetAllocator();
	rapidjson::Value value(version_string, allocator);
	doc.AddMember("Version", value, allocator);

	rapidjson::Value json_categories;
	json_categories.SetArray();
	for (const std::string& category : transaction_model.get_category_names())
	{
		if (category == "None") continue;
		rapidjson::Value json_object;
		json_object.SetObject();
		{
			rapidjson::Value value(category.c_str(), allocator);
			json_object.AddMember("Name", value, allocator);
		}
		{
			rapidjson::Value value(transaction_model.get_category_colors().at(category).to_string().c_str(), allocator);
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
			rapidjson::Value value(account.name.c_str(), allocator);
			json_object.AddMember("Name", value, allocator);
		}
		{
			rapidjson::Value value(account.amount.to_string().c_str(), allocator);
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
	std::ifstream in_file(file_path);
	if (!in_file.is_open())
	{
		cglog::error("Failed to open file \"{}\"", file_path);
		return false;
	}
	std::stringstream file_stream;
	file_stream << in_file.rdbuf();
	std::string file_content = file_stream.str();
	if (file_content.size() == 0) return true;
	std::size_t new_hash = std::hash<std::string>{}(file_content);
	if (new_hash != hash)
	{
		cglog::error("Failed to save file! \"{}\" has been modified outside this program!", file_path);
		return false;
	}
	in_file.close();
	// write file
	std::ofstream out_file(file_path);
	if (!out_file.is_open())
	{
		cglog::error("Failed to open file \"{}\"", file_path);
		return false;
	}
	std::string output = buffer.GetString();
	hash = std::hash<std::string>{}(output);
	out_file << output;
	out_file.close();
	cglog::debug("Wrote to \"{}\"", file_path);
	return true;
}
