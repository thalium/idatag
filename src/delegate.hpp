#pragma once

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QMetaType>
#include <QtWidgets/QListWidget>

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
	QWidget* parent;
	Idatag_configuration* myConfiguration;

public:
	Idatag_delegate_rva(QWidget*, Idatag_model*, Idatag_configuration*);
	void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


class Idatag_delegate_name : public QStyledItemDelegate 
{
	Q_OBJECT
private:
	Idatag_model* myModel;
	QWidget* parent;
	Idatag_configuration* myConfiguration;

public:
	Idatag_delegate_name(QWidget*, Idatag_model*, Idatag_configuration*);
	void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class Idatag_delegate_tag : public QStyledItemDelegate
{
	Q_OBJECT

private:
	Idatag_model* myModel;
	Idatag_palette* myPalette;
	Idatag_proxy* myProxy;
	QWidget* parent;
	Idatag_configuration* myConfiguration;

public:
	Idatag_delegate_tag(QWidget*, Idatag_model*, Idatag_configuration*, Idatag_proxy*);
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void drawTag(Tag tag, QPainter *painter, const QStyleOptionViewItem& option, int width, int height, bool filtered) const;
};

class Idatag_editor 
	: public QLineEdit
{
	Q_OBJECT
private:
	bool lock;
public:
	Idatag_editor(QWidget*);
	void OnEdit();
	void locker();
};