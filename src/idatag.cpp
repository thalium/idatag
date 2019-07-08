#include "idatag.hpp"

bool idaapi run(size_t)
{
	TWidget *ida_widget = find_widget(name);

	const char* path = get_path(PATH_TYPE_IDB);
	if (strlen(path) > 0) //isIDBLoaded
	{
		myConfiguration->load_configuration();

		if (auto_is_ok()) {
			if (myModel == NULL) {
				myModel = new Idatag_model(myConfiguration);
				myHook_IDB = new Idatag_hook_idb();
				myHook_UI = new Idatag_hook_ui();
				myHook_IDP = new Idatag_hook_idp();
				myHook_CView = new Idatag_hook_cview();

				if (myModel == NULL) return true;

				myModel->init_model();
				myModel->print_stats_model();
			}
		}
		else {
			msg("\n[IDATag] Waiting for autoanalysis. Try again later please.");
			return true;
		}
	}
	else
	{
		msg("\n[IDATag] Waiting for file to be loaded. Try again later please.");
		return true;
	}

	if (ida_widget == NULL)
	{
		ida_widget = create_empty_widget(name);
		if (myConfiguration == NULL) return false;

		const char* path = get_path(PATH_TYPE_IDB);
		if (strlen(path) > 0) //isIDBLoaded
		{
			myConfiguration->load_configuration();
		}

		myView = new Idatag_view((QWidget *)ida_widget, myModel, myConfiguration);
		if (myView == NULL) return false;
		display_widget(ida_widget, WOPN_DP_TAB);
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
	msg("-------------------------------------------------");
	msg("\nIDATag v%s - %s - %s", version, authors, date);
	msg("\nIDATag shortcut : Ctrl+Alt+T\n");
	msg("-------------------------------------------------");
	myConfiguration = new Idatag_configuration();
	if (myConfiguration == NULL) return PLUGIN_SKIP;
	return PLUGIN_KEEP;
}

void idaapi term(void)
{
	msg("\n[IDATag] Unloading plugin.");
	
	if(myHook_IDP != NULL) myHook_IDP->~Idatag_hook_idp();
	if(myHook_IDB != NULL) myHook_IDB->~Idatag_hook_idb();
	if(myHook_UI != NULL) myHook_UI->~Idatag_hook_ui();
	if(myHook_CView != NULL) myHook_CView->~Idatag_hook_cview();
	
	if(myConfiguration != NULL) myConfiguration->uninstall_menu_configuration();
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
