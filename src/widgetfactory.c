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

#include "include/common.h"
#include "include/utils.h"
#include "include/luamotif.h"
#include "include/widgetfactory.h"

int CreateManagedWidgetTree(lua_State* L, int parentObj, Widget wdgParent, char* pszWidgetName) {

	WidgetCallback CallbackFunction;
	Widget wdgWidget;
	int iLuaTableID, iUnnamedWidgets = 0;
	char* pszKey;
	char szKeyGenBuf[50];
	bool startManaged = true;

	wdgWidget = NULL;

	lua_pushstring(L, "__widget");
	lua_rawget(L, -2);
	if (lua_type(L, -1) == LUA_TLIGHTUSERDATA) {
		lua_pop(L, 1);
		return 1;
	}
	lua_pop(L, 1);

	// TODO: If table already has widget, abort

	/*
	 *
	 *  First: Call widget constructor.
	 *
	 */
	lua_pushstring(L, "__widgetConstructor");
	lua_rawget(L, -2);
	CallbackFunction = lua_topointer(L,-1);
	lua_pop(L,1);
	wdgWidget = (*CallbackFunction)(L, parentObj, wdgParent, pszWidgetName);

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
