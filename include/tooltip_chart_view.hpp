#pragma once

#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QScatterSeries>
#include <QMouseEvent>
#include <QToolTip>
#include <qdatetime.h>

class TooltipChartView : public QChartView
{
	Q_OBJECT;

public:
	explicit TooltipChartView(QWidget* parent);
	void update(QChart* new_chart, QLineSeries* series, const QDate& starting_date);

protected:
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	QLineSeries* series;
	QScatterSeries* marker;
	QDate starting_date;

	float find_y_value_from_x(float x) const;
};
