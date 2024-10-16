#pragma once

#define Uses_TMenuItem
#define Uses_TSubMenu
#define Uses_TKeys
#define Uses_MsgBox

#include <filesystem>
#include <tvision/tv.h>
#include <tvision/tkeys.h>
#include <vector>

#define SessionsList std::vector<const char*>

TSubMenu& populateXSessions();
TSubMenu& populateWaylandSessions();
