#include "idatag.hpp"

Idatag_configuration* myConfiguration;

bool idaapi run(size_t)
{
	msg("\nIDATag v%s - %s - %s", version, authors, date);
	TWidget *ida_widget = find_widget(name);
	if (ida_widget == NULL)
	{
		ida_widget = create_empty_widget(name);
		if (myConfiguration == NULL) return false;

		Idatag_model* myModel = new Idatag_model(myConfiguration);
		if (myModel == NULL) return false;
		myModel->init_model();

		Idatag_hook_idb* myHook_IDB = new Idatag_hook_idb(myModel);
		Idatag_view* myView = new Idatag_view((QWidget *)ida_widget, myModel, myConfiguration);
		if (myView == NULL) return false;
		display_widget(ida_widget, WOPN_TAB);
	}
	else
	{
		msg("\n[IDATAG] Already loaded. Please close the tab first to run it again.");
	}
	
	return true;
}

int idaapi init(void)
{
	if (!is_idaq())
		return PLUGIN_SKIP;

	myConfiguration = new Idatag_configuration();
	return PLUGIN_KEEP;
}

void idaapi term(void)
{
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
