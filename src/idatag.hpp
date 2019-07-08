#pragma once

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>
#include <auto.hpp>

#include "model.hpp"
#include "view.hpp"
#include "configuration.hpp"
#include "hooks_disas.hpp"

constexpr const char * version = "0.3";
constexpr const char* authors = "Arnaud Gatignol @ Thalium";
constexpr const char* date = "2019";
constexpr const char* name = "IDATag";

static const char help[] = "IDATag";
static const char comment[] = "Analysis helper";
static const char wanted_name[] = "IDATag";
static const char wanted_hotkey[] = "Ctrl+Alt+T";

Idatag_configuration* myConfiguration;
Idatag_model* myModel;
Idatag_view* myView;
Idatag_hook_idb* myHook_IDB;
Idatag_hook_ui* myHook_UI;
Idatag_hook_idp* myHook_IDP;
Idatag_hook_cview* myHook_CView;