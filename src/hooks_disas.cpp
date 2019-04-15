#include "hooks_disas.hpp"

Idatag_hook_idb::Idatag_hook_idb(Idatag_model* myModel)
{
	this->myModel = myModel;

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
	
	Offset* offset = myHook_IDB.myModel->get_offset_byrva(ea);
	if (offset == NULL) return;

	offset->set_name(new_name);

	touch_rva(offset);
}

void evt_byte_patched_h(Idatag_hook_idb& myHook_IDB, va_list args)
{
	uint64_t ea = va_arg(args, ea_t);
	uint32_t old_value = va_arg(args, uint32_t);

	Offset* offset = myHook_IDB.myModel->get_offset_byrva(ea);
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
	std::string new_name = va_arg(args, const char*);

	Offset* offset = myHook_IDB.myModel->get_offset_byrva(ea);
	if (offset == NULL) return;

	touch_rva(offset);
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
	}
	return 0;
}
