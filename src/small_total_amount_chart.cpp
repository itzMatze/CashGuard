#include "small_total_amount_chart.hpp"
#include "transaction_model.hpp"
#include <QCategoryAxis>
#include <QChartView>
#include <QLineSeries>
#include <QMouseEvent>
#include <QScatterSeries>
#include <QToolTip>
#include <QValueAxis>

SmallTotalAmountChartView::SmallTotalAmountChartView(QWidget* parent) : QChartView(parent)
{
	chart = new QChart();
	x_axis = new QCategoryAxis(parent);
	y_axis = new QValueAxis(parent);
	series = new QLineSeries(parent);
	setChart(chart);
	chart->addSeries(series);
	chart->legend()->hide();
	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->setBackgroundBrush(Qt::NoBrush);
	chart->setBackgroundPen(Qt::NoPen);
	chart->setPlotAreaBackgroundVisible(false);
	QPen x_grid_pen(Qt::darkGray);
	x_grid_pen.setStyle(Qt::DashLine);
	x_grid_pen.setWidth(1);
	x_axis->setGridLinePen(x_grid_pen);
	chart->addAxis(x_axis, Qt::AlignBottom);
	series->attachAxis(x_axis);

	QPen y_grid_pen(Qt::darkGray);
	y_grid_pen.setStyle(Qt::DashLine);
	y_grid_pen.setWidth(1);
	y_axis->setGridLinePen(y_grid_pen);
	y_axis->setLabelsVisible(false);
	chart->addAxis(y_axis, Qt::AlignLeft);
	series->attachAxis(y_axis);
}

QDate round_up_month(const QDate& date)
{
	QDate new_date = date.addMonths(1);
	return QDate(new_date.year(), new_date.month(), 1);
}

QDate round_down_month(const QDate& date)
{
	return QDate(date.year(), date.month(), 1);
}

void SmallTotalAmountChartView::update(const TransactionModel& transaction_model, const QDate& starting_date)
{
	// show the last year rounded to always show full months
	int32_t previous_amount = 0;
	int32_t amount = 0;
	int32_t min_amount = std::numeric_limits<int32_t>::max();
	int32_t max_amount = std::numeric_limits<int32_t>::lowest();
	series->clear();
	for (int32_t i = transaction_model.rowCount() - 1; i >= 0; i--)
	{
		std::shared_ptr<Transaction> transaction = transaction_model.get_transaction(i);
		amount += transaction->amount.value;
		int32_t days_old = transaction->date.daysTo(round_up_month(QDate::currentDate()));
		if (days_old <= 365)
		{
			// draw line to old value to prevent diagonal lines
			series->append(365 - days_old, double(previous_amount) / 100.0);
			series->append(365 - days_old, double(amount) / 100.0);
			min_amount = std::min(min_amount, amount);
			max_amount = std::max(max_amount, amount);
		}
		previous_amount = amount;
	}

	QStringList old_labels = x_axis->categoriesLabels();
	for (const QString& label : old_labels) x_axis->remove(label);
	int32_t total_days = 0;
	for (int32_t i = 0; i <= 12; ++i)
	{
		QDate old_date = QDate::currentDate().addMonths(i-11);
		total_days += old_date.daysInMonth();
		x_axis->append(old_date.toString("MMMM"), total_days);
	}
	x_axis->setRange(0.0, 365.0);
	y_axis->setRange(double(min_amount) / 100.0, double(max_amount) / 100.0);
}
