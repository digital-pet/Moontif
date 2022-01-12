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


/* Let it be known that ordered tables don't work because __newindex isn't called when the table is initially being parsed

int newindex(lua_State* L) {

	dumpstack(L);
	printf("\n\n");
	lua_rawset(L, -3);

	//assert(0);
	return 0;
}

int createordered(lua_State* L) {
	lua_newtable(L); //metatable
	lua_newtable(L); // __index
	lua_newtable(L); // korder
	lua_setfield(L, -2, "_korder");
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, newindex);
	lua_setfield(L, -2, "__newindex");
	lua_setmetatable(L, -2);
	//lua_setfield(L, -2, "mt");
	return 0;
}
*/

void builditerator(lua_State* L) {
	static long long iter= 0;
	// look for global __widgetOrder table
	// if none, create it
	// ???
	// profit
	lua_pushinteger(L, iter);
	lua_setfield(L, -2, "__id");
	printf("Widget %llu \n", iter);
	iter++;
	// TODO

	return;
}

int CreateManagedWidgetTree(lua_State* L, int parentObj, Widget wdgParent, char* pszWidgetName, int iDepth) {

	WidgetCallback CallbackFunction;
	Widget wdgWidget;
	int iLuaTableID, iUnnamedWidgets = 0;
	char* pszKey;
	char szKeyGenBuf[50];
	bool startManaged = true;

	wdgWidget = NULL;

	// If table already has widget, abort
	dumpstack(L);
	lua_pushstring(L, "__widget");
	lua_rawget(L, -2);
	if (lua_type(L, -1) == LUA_TLIGHTUSERDATA) {
		lua_pop(L, 1);
		return 1;
	}
	lua_pop(L, 1);

	lua_pushstring(L, "__id");
	printf("Creating widget %s", lua_tostring(L, -1));
	lua_pop(L, 1);

	lua_pushstring(L, "__widgetConstructor");
	lua_rawget(L, -2);
	CallbackFunction = lua_topointer(L,-1);
	lua_pop(L,1);
	wdgWidget = (*CallbackFunction)(L, parentObj, wdgParent, pszWidgetName);

	// If widget == null abort
	if (wdgWidget == NULL) {
		return 2;
	}

	iLuaTableID = lua_gettop(L);
	lua_pushnil(L);
	if (iDepth > 0) {
		iDepth--;
	}
	while (lua_next(L, iLuaTableID) != 0) {
		if ((lua_type(L, -1) == LUA_TTABLE) && (iDepth != 0)) {

			if (lua_type(L, -2) == LUA_TSTRING) {
				pszKey = gc_strdup(lua_tostring(L, -2));
			}
			else {
				snprintf(szKeyGenBuf, 50, "UnnamedWidget_%i_%i",iLuaTableID,iUnnamedWidgets);
				pszKey = gc_strdup(szKeyGenBuf);
				iUnnamedWidgets++;
			}
			
			if (strcmp(pszKey, "__parent")) {
				CreateManagedWidgetTree(L, iLuaTableID, wdgWidget, pszKey, iDepth);
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

int lm_ParseAll(lua_State* L) {
	Widget wdgToplevel;
	char szWidgetName[64];

	wdgToplevel = lm_GetWidget(L, 1);
	if (wdgToplevel == NULL) {
		luaL_argerror(L, 1, "First argument must be the toplevel widget or a widget that already exists");
	}
	lua_remove(L, 1);

	if (!lua_istable(L, 1)) {
		luaL_argerror(L, 1, "Second argument must be the table you want to parse");
	}

	if (!lua_isstring(L, 2)) {
		luaL_argerror(L, 2, "Third argument must be the name of the table as a string");
	}

	strlcpy(szWidgetName, lua_tostring(L,2), sizeof szWidgetName);
	lua_remove(L, 2);

	CreateManagedWidgetTree(L, 0, wdgToplevel, szWidgetName, -1);

	return 0;
}

int lm_Parse(lua_State* L) {
	Widget wdgToplevel;
	char szWidgetName[64];
	int iDepth;

	wdgToplevel = lm_GetWidget(L, 1);
	if (wdgToplevel == NULL) {
		luaL_argerror(L, 1, "First argument must be the toplevel widget or a widget that already exists");
	}
	lua_remove(L, 1);

	if (!lua_istable(L, 1)) {
		luaL_argerror(L, 1, "Second argument must be the table you want to parse");
	}

	if (!lua_isstring(L, 2)) {
		luaL_argerror(L, 2, "Third argument must be the name of the table as a string");
	}

	strlcpy(szWidgetName, lua_tostring(L, 2), sizeof szWidgetName);
	lua_remove(L, 2);

	if (!lua_isinteger(L, 2)) {
		luaL_argerror(L, 2, "Fourth argument must be an int.");
	}
	iDepth = lua_tointeger(L, 2);
	lua_remove(L, 2);

	CreateManagedWidgetTree(L, 0, wdgToplevel, szWidgetName, iDepth);

	return 0;
}

int lm_NewRealize(lua_State* L) {
	Widget wdgToplevel;

	wdgToplevel = lm_GetWidget(L, 1);

	XtRealizeWidget(wdgToplevel);
	return 0;
}
