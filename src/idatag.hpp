#pragma once

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>

#include "model.hpp"
#include "view.hpp"
#include "configuration.hpp"
#include "hooks_disas.hpp"

constexpr const char * version = "0.0.1";
constexpr const char* authors = "Arnaud Gatignol @ Thales";
constexpr const char* date = "2019";
constexpr const char* name = "IDATag";

static const char help[] = "IDATag";
static const char comment[] = "Analysis helper";
static const char wanted_name[] = "IDATag";
static const char wanted_hotkey[] = "Ctrl+Alt+T";