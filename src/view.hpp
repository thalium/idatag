#pragma once

#include <QtWidgets/QTableView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtCore/Qt>
#include <QtGui/QContextMenuEvent>

#include <ida.hpp>

#include "model.hpp"
#include "proxy.hpp"
#include "delegate.hpp"
#include "configuration.hpp"

class Idatag_view : public QT::QWidget{
	Q_OBJECT
private:
	Idatag_model*		myModel;
	Idatag_proxy*		myProxy;
	Idatag_configuration* myConfiguration;
	QWidget*			parent;
	QTableView*			tb;
	QHeaderView*		hheader;
	QCheckBox*			cbox;
	QLineEdit*			tf;
	QLabel*				tfl;
	QShortcut*			sc_filter;
	QGridLayout*		layout;

public:
	Idatag_view(QT::QWidget*, Idatag_model*, Idatag_configuration*);
	~Idatag_view();

	void OnFilter_empty();
	void OnFilter_string();

	void OnNavigate(const QModelIndex&);
	void OnSearch();
};
