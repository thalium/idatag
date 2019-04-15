#pragma once

#include <QtCore\qsortfilterproxymodel.h>
#include <QtCore/Qt>

#include "model.hpp"
#include "configuration.hpp"

class Idatag_proxy : public QT::QSortFilterProxyModel {
	Q_OBJECT
public:
	Idatag_proxy(Idatag_model* myModel);
	~Idatag_proxy();
	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
	void set_filter_empty(Qt::CheckState);
	void set_filter_string(QString);

	friend class Idatag_view;

private:
	Idatag_model*		myModel;
	Qt::CheckState		filter_empty_input;
	QString				filter_string_input;

	bool filter_empty(int source_row, const QModelIndex& source_parent) const;
	bool filter_string(int source_row, const QModelIndex& source_parent) const;
};