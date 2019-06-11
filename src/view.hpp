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
#include <QtWidgets/QDialog>

#include <ida.hpp>

#include "model.hpp"
#include "proxy.hpp"
#include "configuration.hpp"
#include "delegate.hpp"

class Idatag_table : public QTableView {
	Q_OBJECT

public:
	void keyPressEvent(QKeyEvent *);
};

class Idatag_ledit : public QLineEdit {
	Q_OBJECT

public:
	void keyPressEvent(QKeyEvent *);
};

class Idatag_cbox : public QCheckBox {
	Q_OBJECT

public:
	void keyPressEvent(QKeyEvent *);
};

class Idatag_context_view : public QDialog {
	Q_OBJECT
private:
	QGridLayout* menu_layout;
	QPushButton* btn_menu_ok;
	QPushButton* btn_menu_cancel;
	QLineEdit* tags_input;
	QLabel* lbl_rva;
	std::vector<Offset*> offset_selected;
	QLabel* lbl_selection;
	QLabel* lbl_name;

	QShortcut* sc_ok;
	QShortcut* sc_cancel;

public:
	Idatag_context_view(QModelIndexList);
	void context_menu_add_tags();
	void context_menu_pass();
};

class Idatag_view : public QWidget{
	Q_OBJECT
private:
	Idatag_model*		myModel;
	Idatag_proxy*		myProxy;
	QItemSelectionModel* selectionModel;
	Idatag_configuration* myConfiguration;
	QWidget*			parent;
	Idatag_table*			tb;
	QHeaderView*		hheader;
	QHeaderView*		vheader;
	Idatag_cbox*			cbox;
	Idatag_ledit*			tf;
	QLabel*				tfl;
	QShortcut*			sc_filter;
	QGridLayout*		layout;

	QLabel*				hfuncl;
	QLabel*				hoffsetl;
	QLabel*				funcl;
	QLabel*				offsetl;

	QWidget* wnd_filter_feeder;
	QGridLayout* feeder_filter_layout;
	QVBoxLayout* feeder_layout;
	QPushButton* btn_filter_feeder_ok;
	QPushButton* btn_filter_feeder_cancel;

	Idatag_context_view* wnd_context_view;

	QMenu* contextual_menu;
	QAction* add_tag;
	QAction* export_tag;
	QAction* import_tag;
	QAction* filter_feeder;
	QAction* paint_tag;
	QAction* reset_filter;

public:
	Idatag_view(QWidget*, Idatag_model*, Idatag_configuration*);
	~Idatag_view();

	void OnFilter_empty();
	void OnFilter_string();
	void OnFilter_feeder();
	void OnFilter_feeder_update();
	void OnFilter_feeder_show();
	void OnFilter_feeder_pass();

	void OnNavigate(const QModelIndex&);
	void OnSearch();

	void customMenuRequested(QPoint pos);
	void createActions();
	void OnAction_add_tag();
	void OnAction_export_tag();
	void OnAction_import_tag();
	void OnAction_filter_feeder();
	void OnAction_paint_tag();
	void OnAction_reset_filter();

	void set_tag_helper(std::string, std::string);
};

extern Idatag_view* myView;

class Idatag_context_disas : public QDialog {
	Q_OBJECT
private:
	action_activation_ctx_t* ctx;
	QGridLayout* menu_layout;
	QPushButton* btn_menu_ok;
	QPushButton* btn_menu_cancel;
	QLineEdit* tags_input;
	uint64_t rva;
	QLabel* lbl_rva;
	QLabel* lbl_name;
	Offset* offset;

	QShortcut* sc_ok;
	QShortcut* sc_cancel;

public:
	Idatag_context_disas(action_activation_ctx_t*);

	void context_menu_add_tags();
	void context_menu_pass();
};

class Idatag_context_disas_func : public QDialog {
	Q_OBJECT
private:
	action_activation_ctx_t* ctx;
	QGridLayout* menu_layout;
	QPushButton* btn_menu_ok;
	QPushButton* btn_menu_cancel;
	QLineEdit* tags_input;
	uint64_t rva;
	QLabel* lbl_rva;
	QLabel* lbl_name;
	Offset* offset;

	QShortcut* sc_ok;
	QShortcut* sc_cancel;

public:
	Idatag_context_disas_func(action_activation_ctx_t*);

	void context_menu_add_tags();
	void context_menu_pass();
};

class Idatag_context_func : public QDialog {
	Q_OBJECT
private:
	action_activation_ctx_t* ctx;
	QGridLayout* menu_layout;
	QPushButton* btn_menu_ok;
	QPushButton* btn_menu_cancel;
	QLineEdit* tags_input;
	QLabel* lbl_rva;
	std::vector<func_t*> func_selected;
	QLabel* lbl_selection;
	QLabel* lbl_name;

	QShortcut* sc_ok;
	QShortcut* sc_cancel;

public:
	Idatag_context_func(action_activation_ctx_t*);

	void context_menu_add_tags();
	void context_menu_pass();
};

class Idatag_context_name : public QDialog {
	Q_OBJECT
private:
	action_activation_ctx_t* ctx;
	QGridLayout* menu_layout;
	QPushButton* btn_menu_ok;
	QPushButton* btn_menu_cancel;
	QLineEdit* tags_input;
	QLabel* lbl_rva;
	std::vector<uint64_t> name_selected;
	QLabel* lbl_selection;
	QLabel* lbl_name;

	QShortcut* sc_ok;
	QShortcut* sc_cancel;

public:
	Idatag_context_name(action_activation_ctx_t*);

	void context_menu_add_tags();
	void context_menu_pass();
};