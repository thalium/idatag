#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>

#include "idatag.hpp"

//--------------------------------------------------------------------------
#define PLUGIN_VERSION "0.0.1"
#define AUTHORS "Arnaud Gatignol @ Thales"
#define DATE "2019"

//--------------------------------------------------------------------------
static const char help[] = "IDATag";
static const char comment[] = "Analysis helper";
static const char wanted_name[] = "IDATag";
static const char wanted_hotkey[] = "Ctrl+Alt+T";

//--------------------------------------------------------------------------
bool idaapi run(size_t)
{
	msg("IDATag v%s - %s - %s\r\n", PLUGIN_VERSION, AUTHORS, DATE);
	return true;
}

//--------------------------------------------------------------------------
int idaapi init(void)
{
	return PLUGIN_KEEP;
}

//--------------------------------------------------------------------------
void idaapi term(void)
{
}

//--------------------------------------------------------------------------
//
//      PLUGIN DESCRIPTION BLOCK
//
//--------------------------------------------------------------------------
plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION,
	PLUGIN_FIX,    // plugin flags
	init,                 // initialize
	term,                 // terminate. this pointer may be NULL.
	run,                  // invoke plugin
	comment,              // long comment about the plugin
						  // it could appear in the status line
						  // or as a hint
	help,                 // multiline help about the plugin
	wanted_name,          // the preferred short name of the plugin
	wanted_hotkey         // the preferred hotkey to run the plugin
};
