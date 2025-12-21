#include "total_amount_chart.hpp"
#include "transaction_model.hpp"
#include <QCategoryAxis>
#include <QChartView>
#include <QLineSeries>
#include <QMouseEvent>
#include <QScatterSeries>
#include <QToolTip>
#include <QValueAxis>

TotalAmountChartView::TotalAmountChartView(QWidget* parent) : QChartView(parent)
{
	chart = new QChart();
	x_axis = new QCategoryAxis(parent);
	y_axis = new QValueAxis(parent);
	series = new QLineSeries(parent);
	marker = new QScatterSeries(parent);
	setChart(chart);
	chart->addSeries(series);
	chart->addSeries(marker);
	chart->legend()->hide();
	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->setBackgroundBrush(Qt::NoBrush);
	chart->setBackgroundPen(Qt::NoPen);
	chart->setPlotAreaBackgroundVisible(false);
	{
		x_axis->setLabelsColor(Qt::white);
		QFont font = x_axis->labelsFont();
		font.setPointSize(12);
		x_axis->setLabelsFont(font);
		QPen grid_pen(Qt::darkGray);
		grid_pen.setStyle(Qt::DashLine);
		grid_pen.setWidth(1);
		x_axis->setGridLinePen(grid_pen);
		chart->addAxis(x_axis, Qt::AlignBottom);
		series->attachAxis(x_axis);
	}
	{
		y_axis->setLabelsColor(Qt::white);
		QFont font = y_axis->labelsFont();
		font.setPointSize(12);
		y_axis->setLabelsFont(font);
		QPen grid_pen(Qt::darkGray);
		grid_pen.setStyle(Qt::DashLine);
		grid_pen.setWidth(1);
		y_axis->setGridLinePen(grid_pen);
		y_axis->setLabelsVisible(true);
		chart->addAxis(y_axis, Qt::AlignLeft);
		series->attachAxis(y_axis);
	}
	marker->setMarkerSize(8);
	marker->setColor(Qt::red);
	marker->setBorderColor(Qt::red);
	marker->attachAxis(chart->axes().at(0));
	marker->attachAxis(chart->axes().at(1));
}

int64_t find_magnitude(int32_t value)
{
	int64_t magnitude = 10;
	while (magnitude < value) magnitude *= 10;
	return magnitude / 10;
}

void TotalAmountChartView::update(const TransactionModel& transaction_model, const QDate& starting_date)
{
	this->starting_date = starting_date;
	const QDate& date_min = transaction_model.get_filter().date_min;
	const QDate& date_max = transaction_model.get_filter().date_max;
	int32_t total_days = date_min.daysTo(date_max);
	int32_t previous_amount = 0;
	int32_t amount = 0;
	int32_t min_amount = std::numeric_limits<int32_t>::max();
	int32_t max_amount = std::numeric_limits<int32_t>::lowest();
	series->clear();
	for (int32_t i = transaction_model.rowCount() - 1; i >= 0; i--)
	{
		std::shared_ptr<Transaction> transaction = transaction_model.get_transaction(i);
		amount += transaction->amount.value;
		if (transaction->date <= date_max && transaction->date >= date_min)
		{
			int32_t days_old = transaction->date.daysTo(date_max);
			// draw line to old value to prevent diagonal lines
			series->append(total_days - days_old, double(previous_amount) / 100.0);
			series->append(total_days - days_old, double(amount) / 100.0);
			min_amount = std::min(min_amount, amount);
			max_amount = std::max(max_amount, amount);
		}
		previous_amount = amount;
	}
	// x axis
	{
		QStringList old_labels = x_axis->categoriesLabels();
		for (const QString& label : old_labels) x_axis->remove(label);
		QDate label_date = date_min;
		int32_t label_days = date_min.daysInMonth() - date_min.day();
		while (label_date < date_max)
		{
			x_axis->append(label_date.toString("MM.yy"), label_days);
			label_date = label_date.addMonths(1);
			label_days += label_date.daysInMonth();
		}
		x_axis->setRange(0.0, total_days);
	}

	// y axis
	{
		int32_t magnitude = find_magnitude(std::max(std::abs(min_amount), std::abs(max_amount))) / 10;
		min_amount = (min_amount / magnitude) * magnitude;
		max_amount = (max_amount / magnitude) * magnitude + magnitude;
		y_axis->setRange(double(min_amount) / 100.0, double(max_amount) / 100.0);
	}
}

void TotalAmountChartView::mouseMoveEvent(QMouseEvent* event)
{
	QPointF chart_pos = chart->mapToValue(event->pos());
	float x = chart_pos.x();
	float y = find_y_value_from_x(x);
	QPoint tooltip_pos(event->globalPosition().x(), event->globalPosition().y());
	QToolTip::showText(tooltip_pos, QString("%1: %2").arg(QDate(starting_date.addDays(x)).toString("dd.MM.yyyy")).arg(y));
	marker->clear();
	marker->append(x, y);

	QChartView::mouseMoveEvent(event);
}

float TotalAmountChartView::find_y_value_from_x(float x) const
{
	if (series->count() == 0) return 0;
	if (series->count() == 1) return series->at(0).y();
	QPointF prevPoint = series->at(0);
	for (int i = 1; i < series->count(); i++)
	{
		QPointF currentPoint = series->at(i);
		if (x >= prevPoint.x() && x <= currentPoint.x())
		{
			float t = (x - prevPoint.x()) / (currentPoint.x() - prevPoint.x());
			return prevPoint.y() * (1.0 - t) + currentPoint.y() * t;
		}
		prevPoint = currentPoint;
	}

	// x is out of range, return the nearest y-value
	return (x < series->at(0).x()) ? series->at(0).y() : series->at(series->count() - 1).y();
}
