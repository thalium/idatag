#pragma once

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>
#include <moves.hpp>

#include "model.hpp"
#include "view.hpp"
#include "configuration.hpp"

class Idatag_hook_idb {
public:
	Idatag_hook_idb();
	~Idatag_hook_idb();
};

static ssize_t idaapi  idb_evt_h(void*, int, va_list);
void evt_rename_h(Idatag_hook_idb&, va_list);
void evt_byte_patched_h(Idatag_hook_idb&, va_list);
void evt_cmt_changed_h(Idatag_hook_idb&, va_list);
void evt_closebase_h(Idatag_hook_idb&, va_list);
void evt_savebase_h(Idatag_hook_idb&, va_list);


struct show_context_menu_disas_ah_t : public action_handler_t
{
	virtual int idaapi activate(action_activation_ctx_t *);
	virtual action_state_t idaapi update(action_update_ctx_t *);
};

struct show_context_menu_disas_func_ah_t : public action_handler_t
{
	virtual int idaapi activate(action_activation_ctx_t *);
	virtual action_state_t idaapi update(action_update_ctx_t *);
};

struct show_context_menu_func_ah_t : public action_handler_t
{
	virtual int idaapi activate(action_activation_ctx_t *);
	virtual action_state_t idaapi update(action_update_ctx_t *);
};

struct show_context_menu_name_ah_t : public action_handler_t
{
	virtual int idaapi activate(action_activation_ctx_t *);
	virtual action_state_t idaapi update(action_update_ctx_t *);
};

class Idatag_hook_ui {
private:
	action_desc_t desc_disas_menu;
	action_desc_t desc_func_menu;
	action_desc_t desc_name_menu;
	action_desc_t desc_disas_menu_func;

public:
	Idatag_hook_ui();
	~Idatag_hook_ui();

	show_context_menu_disas_ah_t show_context_disas_menu_ah;
	show_context_menu_disas_func_ah_t show_context_disas_menu_func_ah;
	show_context_menu_func_ah_t show_context_func_menu_ah;
	show_context_menu_name_ah_t show_context_name_menu_ah;
};

static ssize_t idaapi  ui_evt_h(void*, int, va_list);
void evt_contextmenu_h(Idatag_hook_ui&, va_list);

class Idatag_hook_idp {
public:
	Idatag_hook_idp();
	~Idatag_hook_idp();
};

static ssize_t idaapi  idp_evt_h(void*, int, va_list);
void evt_newfile_h(Idatag_hook_idp&, va_list);

class Idatag_hook_cview {

public:
	Idatag_hook_cview();
	~Idatag_hook_cview();

};

static ssize_t idaapi  cview_evt_h(void*, int, va_list);
void evt_view_loc_changed_h(Idatag_hook_cview&, va_list);


extern Idatag_hook_idb* myHook_IDB;
extern Idatag_hook_ui* myHook_UI;
extern Idatag_hook_idp* myHook_IDP;
extern Idatag_hook_cview* myHook_CView;