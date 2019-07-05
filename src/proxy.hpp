#pragma once

#include <QtCore/QSortFilterProxyModel>
#include <QtCore/Qt>

#include "model.hpp"
#include "configuration.hpp"

class Idatag_proxy : public QSortFilterProxyModel {
	Q_OBJECT
public:
	Idatag_proxy(Idatag_model* myModel);
	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
	void set_filter_empty(Qt::CheckState);
	void set_filter_string(QString);
	void set_filter_feeder(std::vector<std::string>);
	std::vector<std::string> get_filter_feeder();
	bool is_feeder_filtered(std::string);
	bool is_label_filtered(std::string);
	void reset_filters();

	friend class Idatag_view;

private:
	Idatag_model*		myModel;
	Qt::CheckState		filter_empty_input;
	QString				filter_string_input;
	std::vector<std::string> filter_feeder_input;

	bool filter_empty(int source_row, const QModelIndex& source_parent) const;
	bool filter_string(int source_row, const QModelIndex& source_parent) const;
	bool filter_feeder(int source_row, const QModelIndex& source_parent) const;
};