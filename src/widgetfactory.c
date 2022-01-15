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

// notation:
// !t = source table
// *k = key
// *v = value
// t* = table
// s* = string
// u* = userdata
// f* = function
// x* = nil

int CreateManagedWidgetTree(lua_State* L, int parentObj, Widget wdgParent, char* pszWidgetName, int iDepth) {

	// initial state = !t

	WidgetConstructor Constructor;
	Widget wdgWidget;
	lua_Integer iLuaTableID, *iKey, iTableSize = 0, iChildCount = 0;
	char* pszKey;
	char szKeyGenBuf[MAXKEYSIZE];
	bool startManaged = true;
	tableSortWrapper* aKeyIndices;

	wdgWidget = NULL;

	// If table already has widget, abort
	lua_pushstring(L, "__widget");					// !t sk
	lua_rawget(L, -2);								// !t uv || !t xv
	if (lua_type(L, -1) == LUA_TLIGHTUSERDATA) {	// !t uv?
		lua_pop(L, 1);								// !t
		return 1;
	}
	lua_pop(L, 1);									// !t

	lua_pushstring(L, "__widgetConstructor");		// !t sk
	lua_rawget(L, -2);								// !t uv || xv
	Constructor = lua_topointer(L,-1);				
	lua_pop(L,1);									// !t

	if (Constructor == NULL) {
		return 3;
	}

	wdgWidget = (*Constructor)(L, parentObj, wdgParent, pszWidgetName);

	// If widget == null abort
	if (wdgWidget == NULL) {
		return 2;
	}

	iLuaTableID = lua_gettop(L);
	lua_pushnil(L);								// !t xk
	while (lua_next(L, iLuaTableID) != 0) {		// !t *k -> !t -> !t *k *v
		if (lua_type(L, -1) == LUA_TTABLE) {	// !t *k tv?
			iTableSize++;
		}
		lua_pop(L, 1);							// !t *k
	}											// loop until no addition to stack
												// !t

	if ((iDepth != 0) && (iTableSize > 0)) {
		if (iDepth > 0) {
			iDepth--;
		}

		aKeyIndices = (tableSortWrapper*)GC_MALLOC(sizeof(*aKeyIndices) * iTableSize);

		lua_pushnil(L);								// !t xk
		while (lua_next(L,iLuaTableID) != 0) {		// !t *k -> !t -> !t *k *v

			if (lua_type(L, -1) == LUA_TTABLE) {	// !t *k tv?
				if (lua_type(L, -2) == LUA_TSTRING) {	// !t sk? tv

					pszKey = gc_strdup(lua_tostring(L, -2));
					if (!strcmp(pszKey, "__parent")) {
						lua_pop(L, 1);				// !t sk
						continue;					// loop
					}
					iKey = NULL;
				}
				else if (lua_type(L, -2) == LUA_TNUMBER) {	// !t ik? tv
					iKey = (lua_Integer*)GC_MALLOC(sizeof(lua_Integer));
					*iKey = lua_tointeger(L, -2);
					snprintf(szKeyGenBuf, sizeof(szKeyGenBuf), "Unnamed_%lld_%lld", iLuaTableID, *iKey);
					pszKey = gc_strdup(szKeyGenBuf);
				}
				else {
					lua_pop(L, 1);					// !t *k
					continue;						// loop
				}
				GetRegistry(L);								// !t *k tv !t2
				printf("Expected 4\n");
				dumpstack(L);
				lua_getfield(L, -1, "__widgetOrder");		// !t *k tv !t2 !t3
				printf("Expected 5\n");
				dumpstack(L);
				lua_pushvalue(L, -3);						// !t *k tv !t2 !t3 tv
				lua_rawget(L, -2);							// !t *k tv !t2 !t3 *v
				printf("Expected 6\n");
				dumpstack(L);
				if (lua_type(L, -1) != LUA_TNUMBER) {		// !t *k tv !t2 !t3 iv~?
					lua_pop(L,4);							// !t *k
					continue;
				}

				aKeyIndices[iChildCount].index = lua_tonumber(L, -1);

				aKeyIndices[iChildCount].pszKey = pszKey;

				aKeyIndices[iChildCount].iKey = iKey;


				iChildCount++;

				lua_pop(L, 4);							// !t *k

			}
			lua_pop(L, 1);								// !t

		}

		qsort(aKeyIndices, iChildCount, sizeof(*aKeyIndices), TableSortComparator);

		for (int i2 = 0; i2 < iChildCount; i2++) {
			// recurse over the selected tables

			if (aKeyIndices[i2].iKey == NULL) {
				lua_pushstring(L, aKeyIndices[i2].pszKey);
				lua_rawget(L, -2);
			}
			else {
				lua_pushnumber(L, *aKeyIndices[i2].iKey);
				lua_rawget(L, -2);
			}
			CreateManagedWidgetTree(L, iLuaTableID, wdgWidget, aKeyIndices[i2].pszKey, iDepth);
			lua_pop(L, 1);
		}
	}


	lua_pushstring(L, "startManaged");
	lua_gettable(L, -2);
	if (lua_type(L, -1) == LUA_TBOOLEAN) {
		startManaged = lua_toboolean(L, -1);
	}
	lua_pop(L, 1);


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
	char szWidgetName[MAXKEYSIZE];

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
	char szWidgetName[MAXKEYSIZE];
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

int TableSortComparator(const void* left, const void* right) {
	tableSortWrapper* tLeft = (tableSortWrapper*)left;
	tableSortWrapper* tRight = (tableSortWrapper*)right;

	return (tLeft->index - tRight->index);
}