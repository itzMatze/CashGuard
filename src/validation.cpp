#include "validation.hpp"
#include <qmessagebox.h>

bool validateTransactionIndex(uint32_t index, const TransactionModel& transaction_model, QWidget* parent)
{
	if (index >= 0 && index < transaction_model.rowCount()) return true;
	if (parent) QMessageBox::warning(parent, "Error", "Invalid transaction index!");
	return false;
}
