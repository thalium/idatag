#pragma once

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>

#include "model.hpp"

class Idatag_hook_idb {
public:
	Idatag_model* myModel;
	Idatag_hook_idb(Idatag_model*);
	~Idatag_hook_idb();
};

static ssize_t idaapi  idb_evt_h(void*, int, va_list);

void evt_rename_h(Idatag_hook_idb&, va_list);