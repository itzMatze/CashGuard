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

class TotalAmountChartView : public QChartView
{
	Q_OBJECT;

public:
	explicit TotalAmountChartView(QWidget* parent);
	void update(const TransactionModel& transaction_model, const QDate& starting_date);

protected:
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	QChart* chart;
	QCategoryAxis* x_axis;
	QValueAxis* y_axis;
	QLineSeries* series;
	QScatterSeries* marker;
	QDate starting_date;

	float find_y_value_from_x(float x) const;
};
