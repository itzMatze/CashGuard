#ifndef TRANSACTIONDIALOG_H
#define TRANSACTIONDIALOG_H

#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qlineedit.h>

class TransactionDialog : public QDialog
{
	Q_OBJECT

public:
	TransactionDialog(QWidget *parent = nullptr);
	QDate getDate() const;
	QString getCategory() const;
	QString getAmount() const;
	QString getDescription() const;

private:
	QDateEdit* dateInput;
	QComboBox *categoryInput;
	QLineEdit *amountInput;
	QLineEdit *descriptionInput;
};

#endif // TRANSACTIONDIALOG_H
