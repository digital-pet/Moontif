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

#include <iconv.h>
#include <lua.h>
#include <lauxlib.h>

#include "include/luamotif.h"

int
lm_XmFileSelectionBoxGetChild(lua_State* L)
{
	Widget wdgChild, wdgParent;
	int iChildIndex;

	if (!lua_islightuserdata(L, 1))
		luaL_argerror(L, 1, "udata expected");

	wdgParent = lua_touserdata(L, 1);
	iChildIndex = luaL_checkinteger(L, 2);
	wdgChild = XmFileSelectionBoxGetChild(wdgParent, iChildIndex);
	lua_pushlightuserdata(L, wdgChild);
	return 1;
}

int
lm_XmFileSelectionDoSearch(lua_State* L)
{
	Widget wdgWidget;
	XmString xmsString;

	if (!lua_islightuserdata(L, 1))
		luaL_argerror(L, 1, "udata expected");
	if (!lua_islightuserdata(L, 2))
		luaL_argerror(L, 2, "udata expected");

	wdgWidget = lua_touserdata(L, 1);
	xmsString = lua_touserdata(L, 2);
	XmFileSelectionDoSearch(wdgWidget, xmsString);
	return 0;
}


int
lm_XmMessageBoxGetChild(lua_State* L)
{
	Widget wdgChild, wdgParent;
	int iChildIndex;

	if (!lua_islightuserdata(L, 1))
		luaL_argerror(L, 1, "udata expected");

	wdgParent = lua_touserdata(L, 1);
	iChildIndex = luaL_checkinteger(L, 2);
	wdgChild = XmMessageBoxGetChild(wdgParent, iChildIndex);
	lua_pushlightuserdata(L, wdgChild);
	return 1;
}

int
lm_XmListAddItem(lua_State* L)
{
	Widget wdgList;
	char* pszItemString;
	XmString xmsItemString;

	wdgList = lm_GetWidget(L, 1);
	pszItemString = (char*)luaL_checkstring(L, 2);
	xmsItemString = XmStringCreateLocalized(pszItemString);
	XmListAddItem(wdgList, xmsItemString, luaL_checkinteger(L, 3));
	XmStringFree(xmsItemString);
	return 0;
}


int
lm_ProcessTraversal(lua_State* L)
{
	Widget wdgWidget;
	int iDirection;

	wdgWidget = lm_GetWidget(L, 1);
	iDirection = luaL_checkinteger(L, 2);
	XmProcessTraversal(wdgWidget, (XmTraversalDirection)iDirection);
	return 0;
}

int
lm_GetString(lua_State* L)
{
	Widget wdgWidget;
	char* pszString;

	wdgWidget = lm_GetWidget(L, 1);

	pszString = XmTextGetString(wdgWidget);
	if (pszString == NULL)
		lua_pushnil(L);
	else
		lua_pushstring(L, pszString);
	return 1;
}

int
lm_Remove(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextRemove(wdgWidget);
	return 0;
}

int
lm_Replace(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextReplace(wdgWidget, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
		(char*)luaL_checkstring(L, 4));
	return 0;
}

int
lm_Insert(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);

	XmTextInsert(wdgWidget, luaL_checkinteger(L, 2),
		(char*)luaL_checkstring(L, 3));
	return 0;
}

int
lm_GetInsertionPosition(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	lua_pushinteger(L, XmTextGetInsertionPosition(wdgWidget));
	return 1;
}

/* START */

int
lm_GetLastPosition(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	lua_pushinteger(L, XmTextGetLastPosition(wdgWidget));
	return 1;
}

int
lm_SetInsertionPosition(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextSetInsertionPosition(wdgWidget, luaL_checkinteger(L, 2));
	return 0;
}

int
lm_SetMaxLength(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextSetMaxLength(wdgWidget, luaL_checkinteger(L, 2));
	return 0;
}

int
lm_SetSelection(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);

	XmTextSetSelection(wdgWidget, luaL_checkinteger(L, 2),
		luaL_checkinteger(L, 3),
		XtLastTimestampProcessed(XtDisplay(wdgWidget)));
	return 0;
}

int
lm_SetString(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextSetString(wdgWidget, (char*)luaL_checkstring(L, 2));
	return 0;
}

int
lm_UpdateDisplay(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmUpdateDisplay(wdgWidget);
	return 0;
}

/* START */


int lm_GetValues(lua_State* L)
{
	Arg args[1];
	Widget wdgWidget;
	int iCurrentArgument, iter, iterMax, iArgType;
	const char* pszArgName;
	char* pszArgValue;
	Boolean bArgBool;
	unsigned char uchar;
	Cardinal cardinal;
	Dimension dimension;
	Position position;
	XmString xmsString;

	wdgWidget = lm_GetWidget(L, 1);

	iterMax = lua_gettop(L);
	for (iCurrentArgument = 0, iter = 2; iter <= iterMax; iter++) {
		if (lua_type(L, iter) != LUA_TSTRING)
			continue;

		pszArgName = lua_tostring(L, iter);
		iArgType = get_type(pszArgName);
		switch (iArgType) {
		case BOOLEAN:
			XtSetArg(args[0], (String)pszArgName, &bArgBool);
			XtGetValues(wdgWidget, args, 1);
			lua_pushboolean(L, bArgBool);
			iCurrentArgument++;
			break;
		case UCHAR:
			XtSetArg(args[0], (String)pszArgName, &uchar);
			XtGetValues(wdgWidget, args, 1);
			lua_pushinteger(L, uchar);
			iCurrentArgument++;
			break;
		case CARDINAL:
			XtSetArg(args[0], (String)pszArgName, &cardinal);
			XtGetValues(wdgWidget, args, 1);
			lua_pushinteger(L, cardinal);
			iCurrentArgument++;
			break;
		case DIMENSION:
			XtSetArg(args[0], (String)pszArgName, &dimension);
			XtGetValues(wdgWidget, args, 1);
			lua_pushinteger(L, dimension);
			iCurrentArgument++;
			break;
		case POSITION:
			XtSetArg(args[0], (String)pszArgName, &position);
			XtGetValues(wdgWidget, args, 1);
			lua_pushinteger(L, position);
			iCurrentArgument++;
			break;

		case STRING:
			XtSetArg(args[0], (String)pszArgName, &xmsString);
			XtGetValues(wdgWidget, args, 1);
			XmStringGetLtoR(xmsString, XmFONTLIST_DEFAULT_TAG, &pszArgValue);
			lua_pushstring(L, pszArgValue);
			XmStringFree(xmsString);
			iCurrentArgument++;
			break;
		default:
			break;
		}
	}
	return iCurrentArgument;
}

int lm_SetValues(lua_State* L)
{
	Arg* args;
	Widget widget;
	int narg;

	widget = lm_GetWidget(L, 1);
	narg = lm_getArgs(L, 2, &args);
	XtSetValues(widget, args, narg);
	free(args);
	return 0;
}

int lm_SetStringValue(lua_State* L)
{
	Widget widget;
	size_t len;
	const char* name, * value;

	name = luaL_checkstring(L, 2);
	value = luaL_checklstring(L, 3, &len);
	widget = lm_GetWidget(L, 1);
	XtVaSetValues(widget, XtVaTypedArg, name, XmRString, value, len + 1,
		NULL);
	return 0;
}

int lm_SetKeyboardFocus(lua_State* L)
{
	Widget subtree, descendant;

	subtree = lm_GetWidget(L, 1);
	descendant = lm_GetWidget(L, 2);
	XtSetKeyboardFocus(subtree, descendant);
	return 0;
}

int lm_SetWorkWindow(lua_State* L)
{
	Widget widget, work_window;

	widget = lm_GetWidget(L, 1);
	work_window = lm_GetWidget(L, 2);
	XtVaSetValues(widget, XmNworkWindow, work_window, NULL);
	return 0;
}

int lm_ScrollVisible(lua_State* L)
{
	Widget w, scroll_to;

	w = lm_GetWidget(L, 1);
	scroll_to = lm_GetWidget(L, 2);

	XmScrollVisible(w, scroll_to, 0, 0);
	return 0;
}

/* Return the true XtParent as a widget */
int lm_XtParent(lua_State* L)
{
	Widget parent, widget;

	widget = lm_GetWidget(L, 1);

	parent = XtParent(widget);

	lua_newtable(L);
	lua_pushlightuserdata(L, parent);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);

	return 1;
}

int lm_Popdown(lua_State* L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);
	XtPopdown(widget);
	return 0;
}

int lm_Popup(lua_State* L)
{
	Widget widget;
	int grab;

	widget = lm_GetWidget(L, 1);
	grab = luaL_checkinteger(L, 2);
	XtPopup(widget, grab);
	return 0;
}

int lm_Screen(lua_State* L)
{
	Widget widget;
	Screen* screen;

	widget = lm_GetWidget(L, 1);
	screen = XtScreen(widget);
	lua_pushinteger(L, screen->width);
	lua_pushinteger(L, screen->height);
	return 2;
};

int lm_Window(lua_State* L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);
	lua_pushinteger(L, XtWindow(widget));
	return 1;
};

int lm_XtSetLanguageProc(lua_State* L)
{
	XtAppContext app_context;
	XtLanguageProc proc;
	XtPointer client_data;

	if (lua_isnil(L, 1))
		app_context = NULL;
	else
		app_context = lua_touserdata(L, 1);
	if (lua_isnil(L, 2))
		proc = NULL;
	else
		proc = lua_touserdata(L, 2);
	if (lua_isnil(L, 3))
		client_data = NULL;
	else
		client_data = lua_touserdata(L, 3);
	XtSetLanguageProc(app_context, proc, client_data);
	return 0;
}

int lm_SetSensitive(lua_State* L)
{
	Widget widget;
	int value;

	widget = lm_GetWidget(L, 1);
	value = luaL_checkinteger(L, 2);
	XtSetSensitive(widget, value);
	return 0;
}

int lm_UnmanageChild(lua_State* L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);
	XtUnmanageChild(widget);
	return 0;
}

int lm_ManageChild(lua_State* L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);
	XtManageChild(widget);
	return 0;
}

int lm_Unrealize(lua_State* L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XtUnrealizeWidget(wdgWidget);

	return 0;
}

int lm_GetPixmap(lua_State* L)
{
	Widget wdgWidget, wdgToplevel;
	Pixmap pixmap;

	wdgWidget = lm_GetWidget(L, 1);
	wdgToplevel = lm_GetWidget(L, 2);

	XtVaGetValues(wdgWidget, XmNlabelPixmap, &pixmap, NULL);
	if (pixmap != XmUNSPECIFIED_PIXMAP)
		XmDestroyPixmap(XtScreen(wdgToplevel), pixmap);

	pixmap = XmGetPixmap(XtScreen(wdgToplevel), (char*)luaL_checkstring(L, 3),
		XmUNSPECIFIED_PIXEL, XmUNSPECIFIED_PIXEL);

	XtVaSetValues(wdgWidget, XmNlabelPixmap, pixmap, NULL);

	return 0;
}

int lm_DestroyPixmap(lua_State* L)
{
	Widget wdgWidget, wdgToplevel;
	Pixmap pixmap;

	wdgWidget = lm_GetWidget(L, 1);
	wdgToplevel = lm_GetWidget(L, 2);

	XtVaGetValues(wdgWidget, XmNlabelPixmap, &pixmap, NULL);
	if (pixmap != XmUNSPECIFIED_PIXMAP) {
		XmDestroyPixmap(XtScreen(wdgToplevel), pixmap);
		XtVaSetValues(wdgWidget, XmNlabelPixmap, XmUNSPECIFIED_PIXMAP, NULL);
	}

	return 0;
}

int lm_Realize(lua_State* L)
{
	Widget wdgToplevel;
	char szName[64];
	void* pTable;
	int iLuaTableID;

	wdgToplevel = lm_GetWidget(L, 1);
	if (!lua_istable(L, 2))
		luaL_argerror(L, 2, "table expected");

	if (lua_gettop(L) == 3) {
		if (!lua_isstring(L, 3))
			luaL_argerror(L, 3, "string expected");
		strlcpy(szName, lua_tostring(L, 3), sizeof szName);
		lua_pop(L, 1);
	}
	else {
		pTable = (void*)lua_topointer(L, 2);
		strlcpy(szName, "toplevel", sizeof szName);

#if LUA_VERSION_NUM >= 502
		lua_pushglobaltable(L);
		iLuaTableID = lua_gettop(L);
#else
		t = LUA_GLOBALSINDEX;
#endif
		lua_pushnil(L);
		while (lua_next(L, iLuaTableID) != 0) {
			if (lua_topointer(L, -1) == pTable) {
				switch (lua_type(L, -2)) {
				case LUA_TSTRING:
					strlcpy(szName, lua_tostring(L, -2),
						sizeof szName);
					break;
				case LUA_TNUMBER:
					snprintf(szName, sizeof szName, "%.f",
						lua_tonumber(L, -2));
					break;
				}
			}
			lua_pop(L, 1);
		}
#if LUA_VERSION_NUM >= 502
		lua_pop(L, 1);
#endif
	}
	lm_CreateWidgetHierarchy(L, 0, wdgToplevel, szName);
	XtRealizeWidget(wdgToplevel);
	return 0;
}