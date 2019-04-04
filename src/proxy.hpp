#pragma once

#include <QtCore\qsortfilterproxymodel.h>

#include "model.hpp"
#include "configuration.hpp"

class idatag_proxy : public QT::QSortFilterProxyModel {
	Q_OBJECT
public:
	idatag_proxy(idatag_configuration* myConfiguration, idatag_model* myModel);
	~idatag_proxy();
	bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
};