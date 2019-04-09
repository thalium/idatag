#pragma once

#include <QtWidgets/qstyleditemdelegate.h>
#include <QtWidgets/qwidget.h>
#include <QtGui/qpainter.h>
#include <QtCore/qmetatype.h>
#include <QtWidgets/qlistwidget.h>

#include "model.hpp"
#include "view.hpp"


class Idatag_delegate_rva : public QStyledItemDelegate 
{
	Q_OBJECT
private:
	Idatag_model* myModel;
	QT::QWidget* parent;

public:
	Idatag_delegate_rva(QT::QWidget*, Idatag_model*);
	~Idatag_delegate_rva();
	void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


class Idatag_delegate_name : public QStyledItemDelegate 
{
	Q_OBJECT
private:
	Idatag_model* myModel;
	QT::QWidget* parent;

public:
	Idatag_delegate_name(QT::QWidget*, Idatag_model*);
	~Idatag_delegate_name();
	void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class Idatag_delegate_tag : public QStyledItemDelegate 
{
	Q_OBJECT
private:
	Idatag_model* myModel;
	QT::QWidget* parent;
	QTableView* myView;

public:
	Idatag_delegate_tag(QT::QWidget*, Idatag_model*, QT::QTableView*);
	~Idatag_delegate_tag();
	void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QT::QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


class Idatag_wall : public QListWidget
{
	Q_OBJECT
private:
	QT::QWidget* parent;
	Idatag_model* myModel;
	QTableView* myView;
	QModelIndex index;
	const Offset* offset;
	QHBoxLayout* layout;

public:
	Idatag_wall(QT::QWidget*, Idatag_model* , QTableView*, const QModelIndex&, const Offset*);
	void generate_tag(Tag);
	void clear_tags();
};

class Idatag_graph : public QPushButton
{
	Q_OBJECT
private:
	Idatag_wall* wall;
	Tag tag;

public:
	Idatag_graph(Idatag_wall*, Tag);
};