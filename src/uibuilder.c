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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Portions copyright (c) 2009 - 2018, Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Micro Systems Marc Balmer nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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


#include "include/luamotif.h"
#include "include/callbacks.h"

// Called by CreateWidgetHierarchy when creating PopupShells
static Widget
GetTopShell(Widget wdgWidget)
{
	while (wdgWidget && !XtIsWMShell(wdgWidget)) {
		wdgWidget = XtParent(wdgWidget);
	}

	return wdgWidget;
}

// Called by lm_Realize
static Widget
CreateWidgetHierarchy(lua_State* L, int parentObj, Widget wdgParent, const char* pszArgumentName)
{
	Arg aArgs[MAXARGS];
	WidgetClass class;
	Widget wdgWidget;
	XmString *axmsVal;
	int iXmStringCount;


	struct cb_data* cbdCallback;

	char szKey[64], *pszAttributeName;
	int iter, iCurrentArg, iLuaTableID;
	char* pszUtf8Str;

	wdgWidget = NULL;
	iCurrentArg = 0;
	iXmStringCount = 0;

	lua_pushstring(L, "__widgetClass");
	lua_rawget(L, -2);
	class = (WidgetClass)lua_topointer(L, -1);
	lua_pop(L, 1);

	if (class != NULL) {

		/*
		 *  First pass, collect attributes
		 */

		iLuaTableID = lua_gettop(L);
		lua_pushnil(L);

		axmsVal = ((XmString *)GC_malloc(MAXARGS * sizeof(XmString)));

		while (lua_next(L, iLuaTableID) != 0) {
			switch (lua_type(L, -2)) {
				case LUA_TSTRING:
					strlcpy(szKey, lua_tostring(L, -2), sizeof szKey);
					break;

				case LUA_TNUMBER:
					snprintf(szKey, sizeof szKey, "%.f", lua_tonumber(L, -2));
					break;

				default:
					strlcpy(szKey, pszArgumentName, sizeof szKey);

			}

			/* Processed after widget creation */
			if (!strcmp(szKey, "background") || !strcmp(szKey, "foreground")) {
				lua_pop(L, 1);
				continue;
			}

			switch (lua_type(L, -1)) {
				case LUA_TSTRING:
					pszUtf8Str = (char*)lua_tostring(L, -1);
					pszAttributeName = strdup(szKey);

					/* If the attribute is not value or title, use XmStringCreateLocalized */
					if (strcmp(szKey, "value") && strcmp(szKey, "title")) {
						axmsVal[iXmStringCount] = XmStringCreateLocalized(pszUtf8Str);
						XtSetArg(aArgs[iCurrentArg], pszAttributeName, axmsVal[iXmStringCount]);
						iXmStringCount++;
					}
					else {
						XtSetArg(aArgs[iCurrentArg], pszAttributeName, pszUtf8Str);
					}

					iCurrentArg++;
					break;

				case LUA_TNUMBER:
					pszAttributeName = strdup(szKey);
					XtSetArg(aArgs[iCurrentArg], pszAttributeName, (XtArgVal)lua_tointeger(L, -1));
					iCurrentArg++;
					break;

				case LUA_TBOOLEAN:
					pszAttributeName = strdup(szKey);
					XtSetArg(aArgs[iCurrentArg], pszAttributeName, (XtArgVal)(int)lua_toboolean(L, -1));
					iCurrentArg++;
					break;
			}
			lua_pop(L, 1);
		}
	}

	/*
	 *  Now we create the widget with the attributes we collected
	 *  we have to special case DialogShells and BulletinBoards
	 */

	if (class == xmDialogShellWidgetClass) {
		wdgWidget = XtCreatePopupShell(pszArgumentName, class, GetTopShell(wdgParent), aArgs, iCurrentArg);
	}
	else if (class == xmBulletinBoardWidgetClass) {
		wdgWidget = XmCreateBulletinBoard(wdgParent, (String)pszArgumentName, aArgs, iCurrentArg);
	}
	else if (class != NULL) {
		wdgWidget = XtCreateWidget(pszArgumentName, class, wdgParent, aArgs, iCurrentArg);
	}

	/* Deallocate the XmStrings we allocated earlier. */
	if (iXmStringCount > 0) {
		while (--iXmStringCount >= 0) {
			XmStringFree(axmsVal[iXmStringCount]);
		}
	}

	if (wdgWidget != NULL) {
		lua_pushlightuserdata(L, wdgWidget);
		lua_setfield(L, -2, "__widget");
		luaL_getmetatable(L, WIDGET_METATABLE);
		lua_setmetatable(L, -2);
	}

	/*
	 *	Second pass, set the attributes we didn't set before creating the widget, such as callbacks and foreground/background colors and recurse!
	 */
	iLuaTableID = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, iLuaTableID) != 0) {

		switch (lua_type(L, -2)) {
			case LUA_TSTRING:
				strlcpy(szKey, lua_tostring(L, -2), sizeof szKey);
				break;

			case LUA_TNUMBER:
				snprintf(szKey, sizeof iter, "%.f", lua_tonumber(L, -2));
				break;

			default:
				strlcpy(szKey, pszArgumentName, sizeof szKey);
		}

		switch (lua_type(L, -1)) {
			case LUA_TSTRING:
				if (wdgWidget && (!strcmp(szKey, "background") || !strcmp(szKey, "foreground"))) {
					size_t len;
					const char* pszColor;

					pszColor = luaL_checklstring(L, -1, &len);
					XtVaSetValues(wdgWidget, XtVaTypedArg, szKey, XmRString, pszColor, len + 1, NULL);
				}
				break;

			case LUA_TTABLE:
				if (wdgWidget == NULL) {
					CreateWidgetHierarchy(L, iLuaTableID, wdgParent, szKey);
				}
				else {
					CreateWidgetHierarchy(L, iLuaTableID, wdgWidget, szKey);
				}
				break;

			case LUA_TFUNCTION:
				if (wdgWidget == NULL) {
					break;
				}

				/* XXX maybe leaks memory */
				cbdCallback = GC_malloc(sizeof(struct cb_data));
				if (cbdCallback == NULL) {
					luaL_error(L, "out of memory");
				}
				cbdCallback->L = L;
				lua_pushvalue(L, -1);
				cbdCallback->ref = luaL_ref(L, LUA_REGISTRYINDEX);
				lua_pushvalue(L, iLuaTableID);
				cbdCallback->obj = luaL_ref(L, LUA_REGISTRYINDEX);
				cbdCallback->callback_name = strdup(szKey);
				XtAddCallback(wdgWidget, szKey, lm_Callback, cbdCallback);
				XtAddCallback(wdgWidget, XmNdestroyCallback, lm_DestroyCallback, cbdCallback);
				break;

		}
		lua_pop(L, 1);
	}


	for (iter = 0; iter < iCurrentArg; iter++) {
		free(aArgs[iter].name);
	}

	if (parentObj > 0) {
		lua_pushstring(L, "__parent");
		lua_pushvalue(L, parentObj);
		lua_rawset(L, -3);
		/* lua_setfield(L, -2, "__parent"); */ 
		/* lua_setfield does not work here, it assigns the value __parent to the key parentObj for some reason, which is scary and ugly */
	}

	if (wdgWidget) {
		XtManageChild(wdgWidget);
	}

	return wdgWidget;
}

int lm_Realize(lua_State* L)
{
	Widget wdgToplevel;
	char szClassName[64];
	void* pTable;
	int iLuaTableID;


	wdgToplevel = lm_GetWidget(L, 1);
	
	if (!lua_istable(L, 2)) {
		luaL_argerror(L, 2, "table expected");
	}

	if (lua_gettop(L) == 3) {
		if (!lua_isstring(L, 3)) {
			luaL_argerror(L, 3, "string expected");
		}
		strlcpy(szClassName, lua_tostring(L, 3), sizeof szClassName);
		lua_pop(L, 1);
	} 
	else {
		strlcpy(szClassName, "toplevel", sizeof szClassName);


		/* 
		 * if no window class name is provided, we search the global table to get this table's key, assuming that it's in the global scope  
		 */
		pTable = (void*)lua_topointer(L, 2);
		
		lua_pushglobaltable(L);
		iLuaTableID = lua_gettop(L);

		lua_pushnil(L);
		while (lua_next(L, iLuaTableID) != 0) {
			if (lua_topointer(L, -1) == pTable) {
				switch (lua_type(L, -2)) {
				case LUA_TSTRING:
					strlcpy(szClassName, lua_tostring(L, -2), sizeof szClassName);
					break;
				case LUA_TNUMBER:
					snprintf(szClassName, sizeof szClassName, "%.f", lua_tonumber(L, -2));
					break;
				}
			}
			lua_pop(L, 1);
		}

		lua_pop(L, 1);
	}

	CreateWidgetHierarchy(L, 0, wdgToplevel, szClassName);
	/* TODO: Realizing widgets should be separate from populating and managing widgets due to operations that may need to occur between the two */
	XtRealizeWidget(wdgToplevel);
	return 0;
}

int lm_Unrealize(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XtUnrealizeWidget(wdgWidget);

	return 0;
}

int lm_DestroyWidgetHierarchy(lua_State* L)
{
	Widget wdgWidget;
	int iLuaTableID;

	wdgWidget = NULL;

	lua_getfield(L, -1, "__widget");
	wdgWidget = (Widget)lua_topointer(L, -1);
	lua_pop(L, 1);

	if (lua_type(L, -1) == LUA_TTABLE) {
		lua_pushnil(L);
		lua_setfield(L, -2, "__parent");

		iLuaTableID = lua_gettop(L);
		lua_pushnil(L);
		while (lua_next(L, iLuaTableID) != 0) {
			if (lua_type(L, -1) == LUA_TTABLE)
				lm_DestroyWidgetHierarchy(L);
			lua_pop(L, 1);
		}
	}
	if (wdgWidget != NULL) {
		XtDestroyWidget(wdgWidget);
		lua_pushnil(L);
		lua_setfield(L, -2, "__widget");
	}
	return 0;
}