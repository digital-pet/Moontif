/*
 * This file is part of the luamotif-core distribution (https://github.com/digital-pet/luamotif-core).
 * Copyright (c) 2022 digital-pet.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */

#include <stdlib.h>
#ifdef __linux__
#include <bsd/string.h>
#endif

#include <X11/Xdefs.h>
#include <Xm/XmAll.h>

#include <gc.h>

#include <lua.h>
#include <lauxlib.h>

#include "include/common.h"
#include "include/luamotif.h"
#include "include/callbacks.h"
#include "include/widgetfactory.h"

static char* gc_strdup(const char* s) {
	int size = strlen(s) + 1;
	char* p = GC_MALLOC(size);
	if (p) {
		memcpy(p, s, size - 1); //since GC_MALLOC always zeroes memory, we can copy size -1 and ensure there's a null terminator
	}
	return p;
}

int CreateManagedWidgetTree(lua_State* L, int parentObj, Widget wdgParent, char* pszWidgetName) {

	WidgetFactory WidgetFunc;
	Widget wdgWidget;
	int iLuaTableID, iUnnamedWidgets = 0;
	char* pszKey;
	char szKeyGenBuf[50];
	bool startManaged = true;

	wdgWidget = NULL;

	// TODO: If table already has widget, abort

	/*
	 *
	 *  First: Call widget constructor.
	 *
	 */
	lua_pushstring(L, "__widgetConstructor");
	lua_rawget(L, -2);
	WidgetFunc = lua_topointer(L,-1);
	lua_pop(L,1);
	wdgWidget = (*WidgetFunc)(L, parentObj, wdgParent, pszWidgetName);

	// TODO: If widget == null abort


	iLuaTableID = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, iLuaTableID) != 0) {
		if (lua_type(L, -1) == LUA_TTABLE) {

			if (lua_type(L, -2) == LUA_TSTRING) {
				pszKey = gc_strdup(lua_tostring(L, -2));
			}
			else {
				snprintf(szKeyGenBuf, 50, "UnnamedWidget_%i",iUnnamedWidgets);
				pszKey = gc_strdup(szKeyGenBuf);
				iUnnamedWidgets++;
			}
			
			if (strcmp(pszKey, "__parent")) {
				CreateManagedWidgetTree(L, iLuaTableID, wdgWidget, pszKey);
			}

		
		}
		else if (lua_type(L, -2) == LUA_TSTRING && lua_type(L,-1) == LUA_TBOOLEAN) {
			if (!strcmp(lua_tostring(L, -2), "startManaged")) {
				startManaged = lua_toboolean(L, -1);
			}
		}
		lua_pop(L, 1);
	}

	if (parentObj > 0) {
		lua_pushstring(L, "__parent");
		lua_pushvalue(L, parentObj);
		lua_rawset(L, -3);
	}

	if (startManaged == true) {
		XtManageChild(wdgWidget);
	}

	return 0;
}

int lm_NewRealize(lua_State* L) {
	Widget wdgToplevel;
	char szWidgetName[64];


	wdgToplevel = lm_GetWidget(L, 1);

	if (!lua_istable(L, 2)) {
		luaL_argerror(L, 2, "table expected");
	}

	if (lua_gettop(L) == 3) {
		if (!lua_isstring(L, 3)) {
			luaL_argerror(L, 3, "string expected");
		}
		strlcpy(szWidgetName, lua_tostring(L, 3), sizeof szWidgetName);
		lua_pop(L, 1);
	}
	else {
		strlcpy(szWidgetName, "toplevel", sizeof szWidgetName);
	}

	CreateManagedWidgetTree(L, 0, wdgToplevel, szWidgetName);
	XtRealizeWidget(wdgToplevel);
	return 0;
}

Widget ConstructGenericWidget(lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName, WidgetFac1 WidgetFunc) {
	_lua_stackguard_entry(L);

	Arg aCreationArgs[MAXARGS];
	Widget wdgWidget = NULL;
	XmString* axmsValues;
	int iArgCount = 0, iXmStringCount = 0, iLuaTableID;
	char* pszKey, * pszValue;

	struct cb_data* cbdCallback;

	// Gather arguments by looping over the table
	iLuaTableID = lua_gettop(L);
	lua_pushnil(L);

	axmsValues = (XmString*)GC_MALLOC(MAXARGS * sizeof(XmString));
	if (axmsValues == NULL) {
		luaL_error(L, "memory allocation failed");
	}

	while (lua_next(L, iLuaTableID) != 0) {
		switch (lua_type(L, -2)) {
		case LUA_TSTRING:
			break;

		default:
			lua_pop(L, 1);
			continue;
		}

		pszKey = gc_strdup(lua_tostring(L, -2));
		if (pszKey == NULL) {
			luaL_error(L, "memory allocation failed");
		}

		switch (lua_type(L, -1)) {
		case LUA_TSTRING:
			pszValue = gc_strdup(lua_tostring(L, -1));
			if (pszValue == NULL) {
				luaL_error(L, "memory allocation failed");
			}


			if (!strcmp(pszKey, "value") || !strcmp(pszKey, "title")) {
				XtSetArg(aCreationArgs[iArgCount], pszKey, pszValue);
			}
			else {
				axmsValues[iXmStringCount] = XmStringCreateLocalized(pszValue);
				XtSetArg(aCreationArgs[iArgCount], pszKey, axmsValues[iXmStringCount]);
				iXmStringCount++;
			}
			iArgCount++;
			break;

		case LUA_TNUMBER:
			// TODO: this will fail because lua_tointeger doesn't guarantee it won't be garbage collected once it's off the stack.
			XtSetArg(aCreationArgs[iArgCount], pszKey, lua_tointeger(L, -1));
			iArgCount++;
			break;

		case LUA_TBOOLEAN:
			// TODO: this will fail because lua_toboolean doesn't guarantee it won't be garbage collected once it's off the stack.
			XtSetArg(aCreationArgs[iArgCount], pszKey, lua_toboolean(L, -1));
			iArgCount++;
			break;
		}
		lua_pop(L, 1);
	}

	// Create widget using function pointer with creation arguments

	wdgWidget = (*WidgetFunc)(wdgParent, (String)pszWidgetName, aCreationArgs, iArgCount);

	// Deallocate XmStrings which require deallocation
	if (iXmStringCount > 0) {
		while (--iXmStringCount >= 0) {
			XmStringFree(axmsValues[iXmStringCount]);
		}
	}

	if (wdgWidget == NULL) {
		luaL_error(L, "failed to create widget");
		// TODO: pass some sort of error up the chain
	}

	lua_pushlightuserdata(L, wdgWidget);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);

	// Apply post-creation arguments
	iLuaTableID = lua_gettop(L);
	lua_pushnil(L);

	while (lua_next(L, iLuaTableID) != 0) {
		switch (lua_type(L, -2)) {
		case LUA_TSTRING:
			break;

		default:
			lua_pop(L, 1);
			continue;
		}
		pszKey = gc_strdup(lua_tostring(L, -2));
		// colors would be set here but meh I'll do that later.

		switch (lua_type(L, -1)) {
		case LUA_TFUNCTION:
			cbdCallback = GC_malloc(sizeof(struct cb_data));
			if (cbdCallback == NULL) {
				luaL_error(L, "memory allocation failed");
			}

			cbdCallback->L = L;
			lua_pushvalue(L, -1);

			cbdCallback->ref = luaL_ref(L, LUA_REGISTRYINDEX);
			lua_pushvalue(L, iLuaTableID);
			
			cbdCallback->obj = luaL_ref(L, LUA_REGISTRYINDEX);
			cbdCallback->callback_name = gc_strdup(pszKey);

			XtAddCallback(wdgWidget, pszKey, lm_Callback, cbdCallback);
			XtAddCallback(wdgWidget, XmNdestroyCallback, lm_DestroyCallback, cbdCallback);
			break;
		}
		lua_pop(L, 1);

	}


	// Return widget

	_lua_stackguard_exit(L);
	return wdgWidget;
}

Widget ConstructSimpleMenuBar(lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName) {
	// TODO: Everything
	return;
}
