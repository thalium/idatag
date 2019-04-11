#pragma once

#include <QtWidgets/qstyleditemdelegate.h>
#include <QtWidgets/qwidget.h>
#include <QtGui/qpainter.h>
#include <QtCore/qmetatype.h>
#include <QtWidgets/qlistwidget.h>

#include "model.hpp"
#include "view.hpp"
#include "palette.hpp"

#include <string>
#include <sstream>

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
	Idatag_palette* myPalette;
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
	Idatag_palette* myPalette;
	QModelIndex index;
	Offset* offset;
	QHBoxLayout* layout;

public:
	Idatag_wall(QT::QWidget*, Idatag_model* , QTableView*, Idatag_palette*, const QModelIndex&, Offset*);
	void generate_graph(Tag);
	void clear_tags();
	void mouseDoubleClickEvent(QMouseEvent*);

public slots:
	void edit_wall();
signals:
	void doubleClicked();
};

class Idatag_graph : public QPushButton
{
	Q_OBJECT
private:
	Idatag_wall* wall;
	Idatag_model* myModel;
	QTableView* myView;
	Offset* offset;
	QModelIndex index;
	Tag tag;
	QColor colour;

public:
	Idatag_graph(Idatag_wall*, Idatag_model*, QTableView*, Offset*, const QModelIndex&, Tag, QColor);
	QString get_graph_style(QColor);
	void mouseDoubleClickEvent(QMouseEvent*);
	
public slots:
	void edit_graph();
signals :
	void doubleClicked();
};

class Idatag_wallEditor : public QLineEdit
{
	Q_OBJECT
private:
	Idatag_wall* wall;
	Idatag_model* myModel;
	QTableView* myView;
	QModelIndex index;
	Offset* offset;
public:
	Idatag_wallEditor(Idatag_wall*, Idatag_model*, const QModelIndex&, QTableView*, Offset*);
public slots:
	void add_graph();
};

class Idatag_graphEditor : public QLineEdit
{
	Q_OBJECT
private:
	Idatag_graph* graph;
	Idatag_model* myModel;
	QTableView* myView;
	QModelIndex index;
	Offset* offset;
	QString prev_tag;

public:
	Idatag_graphEditor(Idatag_graph*, Idatag_model*, const QModelIndex&, QTableView*, Offset*);
public slots:
	void replace_graph();
};