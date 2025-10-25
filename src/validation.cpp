#include "validation.hpp"
#include <qmessagebox.h>

bool validate_transaction_index(uint32_t index, const TransactionModel& transaction_model)
{
	if (index >= 0 && index < transaction_model.rowCount()) return true;
	return false;
}
