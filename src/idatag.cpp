#include "idatag.hpp"

bool idaapi run(size_t)
{
	msg("IDATag v%s - %s - %s\r\n", version, authors, date);
	TWidget *ida_widget = find_widget(name);
	if (ida_widget == NULL)
	{
		ida_widget = create_empty_widget(name);
		Idatag_model* myModel = new Idatag_model();
		myModel->init_model();
		Idatag_view* myView = new Idatag_view((QT::QWidget *)ida_widget, myModel);
		display_widget(ida_widget, WOPN_TAB);
	}
	else
	{
		msg("[IDATAG] Already loaded. Please close the tab first to run it again.\n");
	}
	
	return true;
}

int idaapi init(void)
{
	return PLUGIN_KEEP;
}

void idaapi term(void)
{
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
