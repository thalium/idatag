#pragma once

#include <QtWidgets/qstyleditemdelegate.h>
#include <QtWidgets/qwidget.h>

#include "model.hpp"
#include "view.hpp"


class Idatag_delegate_rva : public QStyledItemDelegate {
	Q_OBJECT
private:
	Idatag_model* myModel;
	QT::QWidget parent;
	Idatag_view* myView;

public:
	Idatag_delegate_rva(QT::QWidget* parent, Idatag_model* myModel);
	~Idatag_delegate_rva();
	void paint();
};

class Idatag_delegate_name : public QStyledItemDelegate {
	Q_OBJECT
private:
	Idatag_model* myModel;
	QT::QWidget parent;
	Idatag_view* myView;

public:
	Idatag_delegate_name(QT::QWidget* parent, Idatag_model* myModel);
	~Idatag_delegate_name();
	void paint();
};

class Idatag_delegate_tag : public QStyledItemDelegate {
	Q_OBJECT
private:
	Idatag_model* myModel;
	QT::QWidget parent;
	Idatag_view* myView;

public:
	Idatag_delegate_tag(QT::QWidget* parent, Idatag_model* myModel);
	~Idatag_delegate_tag();
	void paint();
	QT::QWidget createEditor();
};