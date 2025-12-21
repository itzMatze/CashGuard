#pragma once

#include <QChartView>
#include <qdatetime.h>

class TransactionModel;
class QCategoryAxis;
class QLineSeries;
class QMouseEvent;
class QScatterSeries;
class QValueAxis;
class QWidget;

class SmallTotalAmountChartView : public QChartView
{
	Q_OBJECT;

public:
	explicit SmallTotalAmountChartView(QWidget* parent);
	void update(const TransactionModel& transaction_model, const QDate& starting_date);

private:
	QChart* chart;
	QCategoryAxis* x_axis;
	QValueAxis* y_axis;
	QLineSeries* series;
	QDate starting_date;
};
