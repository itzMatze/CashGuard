#include "total_amount.hpp"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <limits>

Amount get_filtered_total_amount(const TransactionModel& transaction_model)
{
	int32_t total_amount = 0;
	for (uint32_t i = 0; i < transaction_model.rowCount(); i++) total_amount += transaction_model.get_transaction(i)->amount.value;
	return Amount(total_amount);
}

Amount get_global_total_amount(const TransactionModel& transaction_model)
{
	int32_t total_amount = 0;
	for (std::shared_ptr<const Transaction> t : transaction_model.get_unfiltered_transactions()) total_amount += t->amount.value;
	return Amount(total_amount);
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

int64_t find_magnitude(int32_t value)
{
	int64_t magnitude = 10;
	while (magnitude < value) magnitude *= 10;
	return magnitude / 10;
}

std::pair<QChart*, QLineSeries*> get_small_total_amount_chart(const TransactionModel& transaction_model)
{
	// show the last year rounded to always show full months
	QLineSeries* series = new QLineSeries();
	int32_t previous_amount = 0;
	int32_t amount = 0;
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
		}
		previous_amount = amount;
	}

	QChart* chart = new QChart();
	chart->legend()->hide();
	chart->addSeries(series);
	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->setBackgroundBrush(Qt::NoBrush);
	chart->setBackgroundPen(Qt::NoPen);
	chart->setPlotAreaBackgroundVisible(false);

	QCategoryAxis* x_axis = new QCategoryAxis();
	int32_t total_days = 0;
	for (int32_t i = 0; i <= 12; ++i)
	{
		QDate old_date = QDate::currentDate().addMonths(i-11);
		total_days += old_date.daysInMonth();
		x_axis->append(old_date.toString("MMMM"), total_days);
	}
	x_axis->setRange(0.0, 365.0);
	QPen x_grid_pen(Qt::darkGray);
	x_grid_pen.setStyle(Qt::DashLine);
	x_grid_pen.setWidth(1);
	x_axis->setGridLinePen(x_grid_pen);
	chart->addAxis(x_axis, Qt::AlignBottom);
	series->attachAxis(x_axis);

	QValueAxis* y_axis = new QValueAxis();
	QPen y_grid_pen(Qt::darkGray);
	y_grid_pen.setStyle(Qt::DashLine);
	y_grid_pen.setWidth(1);
	y_axis->setGridLinePen(y_grid_pen);
	y_axis->setLabelsVisible(false);
	chart->addAxis(y_axis, Qt::AlignLeft);
	series->attachAxis(y_axis);

	return {chart, series};
}

std::pair<QChart*, QLineSeries*> get_total_amount_chart(const TransactionModel& transaction_model)
{
	const QDate& date_min = transaction_model.get_filter().date_min;
	const QDate& date_max = transaction_model.get_filter().date_max;
	int32_t total_days = date_min.daysTo(date_max);
	QLineSeries* series = new QLineSeries();
	int32_t previous_amount = 0;
	int32_t amount = 0;
	int32_t min_amount = std::numeric_limits<int32_t>::max();
	int32_t max_amount = std::numeric_limits<int32_t>::min();
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

	QChart* chart = new QChart();
	chart->legend()->hide();
	chart->addSeries(series);
	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->setBackgroundBrush(Qt::NoBrush);
	chart->setBackgroundPen(Qt::NoPen);
	chart->setPlotAreaBackgroundVisible(false);

	// x axis
	{
		QCategoryAxis* axis = new QCategoryAxis();
		axis->setLabelsColor(Qt::white);
		QFont font = axis->labelsFont();
		font.setPointSize(12);
		axis->setLabelsFont(font);
		QDate label_date = date_min;
		int32_t label_days = date_min.daysInMonth() - date_min.day();
		while (label_date < date_max)
		{
			axis->append(label_date.toString("MM.yy"), label_days);
			label_date = label_date.addMonths(1);
			label_days += label_date.daysInMonth();
		}
		axis->setRange(0.0, total_days);
		QPen grid_pen(Qt::darkGray);
		grid_pen.setStyle(Qt::DashLine);
		grid_pen.setWidth(1);
		axis->setGridLinePen(grid_pen);
		chart->addAxis(axis, Qt::AlignBottom);
		series->attachAxis(axis);
	}

	// y axis
	{
		QValueAxis* axis = new QValueAxis();
		axis->setLabelsColor(Qt::white);
		QFont font = axis->labelsFont();
		font.setPointSize(12);
		axis->setLabelsFont(font);

		int32_t magnitude = find_magnitude(std::max(std::abs(min_amount), std::abs(max_amount))) / 10;
		min_amount = (min_amount / magnitude) * magnitude;
		max_amount = (max_amount / magnitude) * magnitude + magnitude;
		axis->setRange(double(min_amount) / 100.0, double(max_amount) / 100.0);

		QPen grid_pen(Qt::darkGray);
		grid_pen.setStyle(Qt::DashLine);
		grid_pen.setWidth(1);
		axis->setGridLinePen(grid_pen);
		axis->setLabelsVisible(true);
		chart->addAxis(axis, Qt::AlignLeft);
		series->attachAxis(axis);
	}

	return {chart, series};
}
