#include "idatag.hpp"

bool idaapi run(size_t)
{
	TWidget *ida_widget = find_widget(name);
	msg("\nIDATag v%s - %s - %s", version, authors, date);
	if (ida_widget == NULL)
	{
		ida_widget = create_empty_widget(name);
		if (myConfiguration == NULL) return false;

		myView = new Idatag_view((QWidget *)ida_widget, myModel, myConfiguration);
		if (myView == NULL) return false;
		display_widget(ida_widget, WOPN_TAB);
	}
	else
	{
		msg("\n[IDATag] Already loaded. Please close the tab first to run it again.");
	}
	
	return true;
}

int idaapi init(void)
{
	if (!is_idaq())
		return PLUGIN_SKIP;
	
	
	myConfiguration = new Idatag_configuration();
	if (myConfiguration == NULL) return PLUGIN_SKIP;

	myModel = new Idatag_model(myConfiguration);
	if (myModel == NULL) return PLUGIN_SKIP;

	myHook_IDB = new Idatag_hook_idb();
	myHook_UI = new Idatag_hook_ui();
	myHook_IDP = new Idatag_hook_idp();

	return PLUGIN_KEEP;
}

void idaapi term(void)
{
	myHook_IDP->~Idatag_hook_idp();
	myHook_IDB->~Idatag_hook_idb();
	myHook_UI->~Idatag_hook_ui();
	myConfiguration->uninstall_menu_configuration();
	TWidget *ida_widget = find_widget(name);
	close_widget(ida_widget, WCLS_SAVE);
}


plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION,
	PLUGIN_FIX,
	init,
	term,
	run,
	comment,
	help,
	wanted_name,
	wanted_hotkey
};
