#include "hooks_disas.hpp"

Idatag_hook_idb::Idatag_hook_idb()
{
	hook_to_notification_point(HT_IDB, &idb_evt_h, this);
}

Idatag_hook_idb::~Idatag_hook_idb()
{
	unhook_from_notification_point(HT_IDB, &idb_evt_h, this);
}

void touch_rva(Offset* offset)
{
	std::string autofeed = "IDATag";
	std::string user = myConfiguration->get_username_configuration();
	Tag tag = Tag(user, autofeed);
	offset->add_tag(tag);
}

void evt_rename_h(Idatag_hook_idb& myHook_IDB, va_list args)
{
	uint64_t ea = va_arg(args, ea_t);
	std::string new_name = va_arg(args, const char*);
	
	Offset* offset = myModel->get_offset_byrva(ea);
	if (offset == NULL) return;

	if (!new_name.empty()) offset->set_name(new_name);
	
	touch_rva(offset);
}

void evt_byte_patched_h(Idatag_hook_idb& myHook_IDB, va_list args)
{
	uint64_t ea = va_arg(args, ea_t);
	uint32_t old_value = va_arg(args, uint32_t);

	Offset* offset = myModel->get_offset_byrva(ea);
	if (offset == NULL) return;

	touch_rva(offset);

	std::string user = myConfiguration->get_username_configuration();
	std::string label = "byte_patched";
	Tag tag = Tag(label, user);
	offset->add_tag(tag);
}

void evt_cmt_changed_h(Idatag_hook_idb& myHook_IDB, va_list args)
{
	uint64_t ea = va_arg(args, ea_t);

	Offset* offset = myModel->get_offset_byrva(ea);
	if (offset == NULL) return;

	touch_rva(offset);

	std::string user = myConfiguration->get_username_configuration();
	std::string label = "commented";
	Tag tag = Tag(label, user);
	offset->add_tag(tag);
}

void evt_closebase_h(Idatag_hook_idb& myHook_IDB, va_list args)
{
	if (myModel == NULL) return;
	msg("\n[IDATag] Closing base. Exporting tags...");
	myModel->export_tags();
}

void evt_savebase_h(Idatag_hook_idb& myHook_IDB, va_list args)
{
	if (myModel == NULL) return;
	msg("\n[IDATag] Saving base. Exporting tags...");
	myModel->export_tags();
}

static ssize_t idaapi idb_evt_h(void* user_data, int notification_code, va_list args)
{
	Idatag_hook_idb* myHook_IDB = static_cast<Idatag_hook_idb*>(user_data);
	if (myHook_IDB == NULL) return 0;

	idb_event::event_code_t event = static_cast<idb_event::event_code_t>(notification_code);
	switch (event)
	{
		case idb_event::event_code_t::renamed: evt_rename_h(*myHook_IDB, args); break;
		case idb_event::event_code_t::byte_patched: evt_byte_patched_h(*myHook_IDB, args); break;
		case idb_event::event_code_t::cmt_changed: evt_cmt_changed_h(*myHook_IDB, args); break;
		case idb_event::event_code_t::closebase: evt_closebase_h(*myHook_IDB, args); break;
		case idb_event::event_code_t::savebase: evt_savebase_h(*myHook_IDB, args); break;
	}
	return 0;
}

int idaapi show_context_menu_disas_ah_t::activate(action_activation_ctx_t *ctx)
{
	Idatag_context_disas* context_menu = new Idatag_context_disas(ctx);
	if (ctx->cur_ea == BADADDR) return 0;
	
	context_menu->show();
	return 0;
}

action_state_t idaapi show_context_menu_disas_ah_t::update(action_update_ctx_t *)
{
	return AST_ENABLE_ALWAYS;
}

int idaapi show_context_menu_disas_func_ah_t::activate(action_activation_ctx_t *ctx)
{
	Idatag_context_disas_func* context_menu = new Idatag_context_disas_func(ctx);
	if (myModel->is_in_func(ctx->cur_ea) != BADADDR)
	{
		context_menu->show();
	}
	else {
		msg("\n[IDATag] Offset not in function. Aborted.");
	}
	return 0;
}

action_state_t idaapi show_context_menu_disas_func_ah_t::update(action_update_ctx_t *)
{
	return AST_ENABLE_ALWAYS;
}

int idaapi show_context_menu_func_ah_t::activate(action_activation_ctx_t *ctx)
{
	Idatag_context_func* context_menu = new Idatag_context_func(ctx);
	context_menu->show();
	return 0;
}

action_state_t idaapi show_context_menu_func_ah_t::update(action_update_ctx_t *)
{
	return AST_ENABLE_ALWAYS;
}

int idaapi show_context_menu_name_ah_t::activate(action_activation_ctx_t *ctx)
{
	Idatag_context_name* context_menu = new Idatag_context_name(ctx);
	context_menu->show();
	return 0;
}

action_state_t idaapi show_context_menu_name_ah_t::update(action_update_ctx_t *)
{
	return AST_ENABLE_ALWAYS;
}

Idatag_hook_ui::Idatag_hook_ui()
{
	hook_to_notification_point(HT_UI, &ui_evt_h, this);

	this->desc_disas_menu = ACTION_DESC_LITERAL(
		"idatag:context_menu_disas",
		"[IDATag] ~A~dd tags",
		&show_context_disas_menu_ah,
		"Shift+T",
		NULL, -1);
	if (!register_action(desc_disas_menu))
	{
		msg("\n[IDATag] Failed to register contextual menu in IDA");
	}

	this->desc_disas_menu_func = ACTION_DESC_LITERAL(
		"idatag:context_menu_disas_func",
		"[IDATag] ~A~dd tags on function",
		&show_context_disas_menu_func_ah,
		NULL,
		NULL, -1);
	if (!register_action(desc_disas_menu_func))
	{
		msg("\n[IDATag] Failed to register contextual menu in IDA");
	}

	this->desc_func_menu = ACTION_DESC_LITERAL(
		"idatag:context_menu_func",
		"[IDATag] ~A~dd tags",
		&show_context_func_menu_ah,
		NULL,
		NULL, -1);
	if (!register_action(desc_func_menu))
	{
		msg("\n[IDATag] Failed to register contextual menu in IDA");
	}

	this->desc_name_menu = ACTION_DESC_LITERAL(
		"idatag:context_menu_name",
		"[IDATag] ~A~dd tags",
		&show_context_name_menu_ah,
		NULL,
		NULL, -1);
	if (!register_action(desc_name_menu))
	{
		msg("\n[IDATag] Failed to register contextual menu in IDA");
	}
}

Idatag_hook_ui::~Idatag_hook_ui()
{
	unhook_from_notification_point(HT_UI, &ui_evt_h, this);
}

void evt_contextmenu_h(Idatag_hook_ui& myHook_UI, va_list args)
{
	TWidget *f = va_arg(args, TWidget *);
	TPopupMenu *p;
	switch (get_widget_type(f))
	{
	case BWN_DISASMS:
		p = va_arg(args, TPopupMenu *);
		if (!attach_action_to_popup(f, p, "idatag:context_menu_disas"))
		{
			msg("\n[IDATag] Failed to attach contextual menu in IDA");
		}
		if (!attach_action_to_popup(f, p, "idatag:context_menu_disas_func"))
		{
			msg("\n[IDATag] Failed to attach contextual menu in IDA");
		}
		break;
	case BWN_NAMES:
		p = va_arg(args, TPopupMenu *);
		if (!attach_action_to_popup(f, p, "idatag:context_menu_name"))
		{
			msg("\n[IDATag] Failed to attach contextual menu in IDA");
		}
		break;
	case BWN_FUNCS :
		p = va_arg(args, TPopupMenu *);
		if (!attach_action_to_popup(f, p, "idatag:context_menu_func"))
		{
			msg("\n[IDATag] Failed to attach contextual menu in IDA");
		}
		break;
	}
}

static ssize_t idaapi ui_evt_h(void* user_data, int notification_code, va_list args)
{
	Idatag_hook_ui* myHook_UI = static_cast<Idatag_hook_ui*>(user_data);
	if (myHook_UI == NULL) return 0;

	switch (notification_code)
	{
		case ui_finish_populating_widget_popup: evt_contextmenu_h(*myHook_UI, args); break;
	}
	return 0;
}

Idatag_hook_idp::Idatag_hook_idp()
{
	hook_to_notification_point(HT_IDP, &idp_evt_h, this);
}

Idatag_hook_idp::~Idatag_hook_idp()
{
	unhook_from_notification_point(HT_IDP, &idp_evt_h, this);
}

void evt_newfile_h(Idatag_hook_idp& myHook_IDP, va_list args)
{
	myModel->export_tags();
	msg("\n[IDATag] New file event not handled yet.");
	/* 
		- Reload configuration
		- Reload model
	*/
}

static ssize_t idaapi idp_evt_h(void* user_data, int notification_code, va_list args)
{
	Idatag_hook_idp* myHook_IDP = static_cast<Idatag_hook_idp*>(user_data);
	if (myHook_IDP == NULL) return 0;
	
	switch (notification_code)
	{
		case processor_t::ev_newfile: evt_newfile_h(*myHook_IDP, args); break;
		case processor_t::ev_oldfile: evt_newfile_h(*myHook_IDP, args); break;
		case processor_t::ev_newbinary: evt_newfile_h(*myHook_IDP, args); break;
	}
	return 0;
}

Idatag_hook_cview::Idatag_hook_cview()
{
	hook_to_notification_point(HT_VIEW, &cview_evt_h, this);
}

Idatag_hook_cview::~Idatag_hook_cview()
{
	unhook_from_notification_point(HT_VIEW, &cview_evt_h, this);
}

void evt_view_loc_changed_h(Idatag_hook_cview& myHook_CView, va_list args)
{
	TWidget* view = va_arg(args, TWidget*);
	const lochist_entry_t* now = va_arg(args, const lochist_entry_t *);
	const lochist_entry_t* was = va_arg(args, const lochist_entry_t *);
	
	std::string tags_off = "-";
	std::string tags_func = "-";

	if (now->plce->toea() != was->plce->toea())
	{
		ea_t curea = now->plce->toea();
		Offset* offset = myModel->get_offset(curea);
		if (offset != NULL) 
		{
			tags_off = offset->get_tags_tostr();
		}

		ea_t func = myModel->is_in_func(curea);
		if (func != BADADDR)
		{
			Offset* offset_func = myModel->get_offset(func);
			if (offset_func != NULL)
			{
				tags_func = offset_func->get_tags_tostr();
			}
		}

		myView->set_tag_helper(tags_off, tags_func);
	}

}

static ssize_t idaapi cview_evt_h(void* user_data, int notification_code, va_list args)
{
	Idatag_hook_cview* myHook_CView = static_cast<Idatag_hook_cview*>(user_data);
	if (myHook_CView == NULL) return 0;

	switch (notification_code)
	{
		case view_notification_t::view_loc_changed: evt_view_loc_changed_h(*myHook_CView, args); break;
	}
	return 0;
}