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
	Idatag_configuration* myConfiguration;

public:
	Idatag_delegate_rva(QT::QWidget*, Idatag_model*, Idatag_configuration*);
	~Idatag_delegate_rva();
	void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


class Idatag_delegate_name : public QStyledItemDelegate 
{
	Q_OBJECT
private:
	Idatag_model* myModel;
	QT::QWidget* parent;
	Idatag_configuration* myConfiguration;

public:
	Idatag_delegate_name(QT::QWidget*, Idatag_model*, Idatag_configuration*);
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
	Idatag_configuration* myConfiguration;

public:
	Idatag_delegate_tag(QT::QWidget*, Idatag_model*, QT::QTableView*, Idatag_configuration*);
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
	Idatag_configuration* myConfiguration;

	QMenu* contextual_menu;
	QAction* export_tag;
	QAction* import_tag;
	QAction* filter_feeder;
	QAction* paint_tag;

	void createActions();

private slots:
	void OnAction_export_tag();
	void OnAction_import_tag();
	void OnAction_filter_feeder();
	void OnAction_paint_tag();

public:
	Idatag_wall(QT::QWidget*, Idatag_model* , QTableView*, Idatag_palette*, const QModelIndex&, Offset*, Idatag_configuration*);
	void generate_graph(Tag);
	void clear_tags();
	void mouseDoubleClickEvent(QMouseEvent*);

	bool eventFilter(QObject *, QEvent *);

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
	Idatag_configuration* myConfiguration;

public:
	Idatag_graph(Idatag_wall*, Idatag_model*, QTableView*, Offset*, const QModelIndex&, Tag, QColor, Idatag_configuration*);
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
	Idatag_configuration* myConfiguration;

public:
	Idatag_wallEditor(Idatag_wall*, Idatag_model*, const QModelIndex&, QTableView*, Offset*, Idatag_configuration*);
	void keyPressEvent(QKeyEvent *event);
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
	Idatag_configuration* myConfiguration;

public:
	Idatag_graphEditor(Idatag_graph*, Idatag_model*, const QModelIndex&, QTableView*, Offset*, Idatag_configuration*);
	void keyPressEvent(QKeyEvent *event);
public slots:
	void replace_graph();
};