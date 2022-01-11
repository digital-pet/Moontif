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
#include <iconv.h>
#include <lua.h>
#include <lauxlib.h>

#include "include/common.h"

#include "include/luamotif.h"
#include "include/dialog.h"
#include "include/callbacks.h"
#include "include/wrapped_funcs.h"
#include "include/application.h"
#include "include/uibuilder.h"
#include "include/widgetfactory.h"

//extern struct luaL_Reg lm_gadgetConstructors[];
extern struct luaL_Reg lm_widgetConstructors[];

extern struct str_constant motif_strings[];
extern struct int_constant motif_ints[];
extern size_t num_motif_strings(void);
extern size_t num_motif_ints(void);

Widget lm_GetWidget(lua_State *L, int iLuaTableID)
{
	Widget wdgWidget;

	if (!lua_istable(L, iLuaTableID))
		luaL_argerror(L, iLuaTableID, "table expected");

	lua_pushstring(L, "__widget");
	lua_rawget(L, iLuaTableID);
	wdgWidget = (Widget)lua_topointer(L, -1);
	lua_pop(L, 1);

	if (wdgWidget == NULL)
		luaL_argerror(L, iLuaTableID, "widget expected");

	return wdgWidget;
}


/*
 *
 * THIS FUNCTION LEAKS MEMORY
 *
 */
int lm_getArgs(lua_State *L, int iStartPosition, Arg **args)
{
	XmString xmsString;
	int iCurrentArgument, iter;
	const char *pszArgName;

	*args = calloc(sizeof(Arg), lua_gettop(L));
	if (*args == NULL)
		luaL_error(L, "memory error");

	for (iCurrentArgument = 0, iter = iStartPosition; iter <= lua_gettop(L); iter++) {
		if (lua_type(L, iter) != LUA_TSTRING)
			continue;
		pszArgName = lua_tostring(L, iter);
		iter++;
		switch (lua_type(L, iter)) {
		case LUA_TNUMBER:
			XtSetArg((*args)[iCurrentArgument], (String)pszArgName, (XtArgVal)
			    (int)lua_tonumber(L, iter));
			iCurrentArgument++;
			break;
		case LUA_TSTRING:
			/* XXX leaks memory */
			xmsString = XmStringCreateLocalized(
			    (char *)lua_tostring(L, iter));
			XtSetArg((*args)[iCurrentArgument], (String)pszArgName, (XtArgVal)xmsString);
			iCurrentArgument++;
			break;
		case LUA_TFUNCTION:
			break;
		case LUA_TNIL:
			break;
		case LUA_TBOOLEAN:
			XtSetArg((*args)[iCurrentArgument], (String)pszArgName, (XtArgVal)
			    (int)lua_toboolean(L, iter));
			iCurrentArgument++;
			break;
		case LUA_TTABLE:
			break;
		case LUA_TUSERDATA:
			XtSetArg((*args)[iCurrentArgument], (String)pszArgName,
			    (XtArgVal)lua_topointer(L, iter));
			iCurrentArgument++;
			break;
		case LUA_TTHREAD:
			break;
		case LUA_TLIGHTUSERDATA:
			XtSetArg((*args)[iCurrentArgument], (String)pszArgName,
			    (XtArgVal)lua_topointer(L, iter));
			iCurrentArgument++;
			break;
		default:
			break;
		}
	}
	return iCurrentArgument;
}

/* Return the true widget */
static int lm_Widget(lua_State *L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);

	lua_pushlightuserdata(L, widget);

	return 1;
}

/* Return the Parent in the Lua object hierarchy */
static int lm_Parent(lua_State *L)
{
	if (!lua_istable(L, -1))
		luaL_argerror(L, -1, "table expected");

	lua_pushstring(L, "__parent");
	lua_rawget(L, -2);
	return 1;
}

static int lm_Initialize(lua_State *L)
{
	Widget toplevel;
	XtAppContext *app;
	String *res, *r;
	const char *application_class;
	int argc = 0, nres, t;
	String *argv;

	res = NULL;
	argv = NULL;
	nres = 0;

	application_class = luaL_checkstring(L, 1);
	if (lua_gettop(L) > 2) {
		/* Count strings */
		t = lua_gettop(L);
		lua_pushnil(L);
		while (lua_next(L, t) != 0) {
			if (lua_type(L, -1) == LUA_TSTRING)
				nres++;
			lua_pop(L, 1);
		}
		if (nres > 2) {
			nres -= 2;
			argc = nres + 1;
			argv = calloc(nres + 2, sizeof(String));
			argv[0] = strdup("lua");
			/* Populate arguments */
			t = lua_gettop(L);
			r = &argv[1];
			lua_pushnil(L);
			while (lua_next(L, t) != 0) {
				if (lua_type(L, -1) == LUA_TSTRING && nres--)
						*r++ = strdup(lua_tostring(L,
						    -1));
				lua_pop(L, 1);
			}
			*r = NULL;
		}
		lua_pop(L, 1);
	}
	nres = 0;
	if (lua_gettop(L) > 1) {
		/* Count strings */
		t = lua_gettop(L);
		lua_pushnil(L);
		while (lua_next(L, t) != 0) {
			if (lua_type(L, -1) == LUA_TSTRING)
				nres++;
			lua_pop(L, 1);
		}
		if (nres > 0) {
			res = calloc(nres + 1, sizeof(String));

			/* Populate resources */
			t = lua_gettop(L);
			r = res;
			lua_pushnil(L);
			while (lua_next(L, t) != 0) {
				if (lua_type(L, -1) == LUA_TSTRING)
					*r++ = strdup(lua_tostring(L, -1));
				lua_pop(L, 1);
			}
			*r = NULL;
		}
	}
	if (argv == NULL) {
		argc = 0;
		argv = GC_malloc(sizeof(String));
		argv[0] = NULL;
	}
	app = lua_newuserdata(L, sizeof(XtAppContext *));
	luaL_getmetatable(L, CONTEXT_METATABLE);
	lua_setmetatable(L, -2);

	toplevel = XtOpenApplication(app, (char *)application_class, NULL, 0,
	    &argc, argv, res, applicationShellWidgetClass, NULL, 0);

	lua_newtable(L);
	lua_pushlightuserdata(L, toplevel);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);
	return 2;
}

static void lm_set_info(lua_State *L) {
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright 2022 digital-pet; Portions copyright (C) 2009 - 2018 micro systems marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "luamotif-core");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "1.0.0");
	lua_settable(L, -3);
}

static void lm_register(lua_State *L, struct luaL_Reg *reg)
{
	while (reg->name != NULL) {
		lua_pushcfunction(L, reg->func);
		lua_setfield(L, -2, reg->name);
		reg++;
	}
}

static int lm_index(lua_State *L)
{
	const char *nam;

	nam = lua_tostring(L, -1);
	lua_rawget(L, -2);
	if (lua_type(L, -1) == LUA_TNIL) {
		if (lua_getmetatable(L, -2)) {
			lua_pushstring(L, nam);
			lua_rawget(L, -2);
			/* XXX if no function found, try attributes */
		}
	}
	return 1;
}

static int lm_newindex(lua_State *L) 
{
	Arg aArgs[1];
	Widget wdgWidget;
	lm_callbackdata *pcbdCallback;
	XmString xmsStr;
	int iCurrentArg, iter;
	const char *pszLuaName;
	char *pszName;

	pszLuaName = lua_tostring(L, -2);

	lua_pushstring(L, "__widget");
	lua_rawget(L, 1);
	wdgWidget = (Widget)lua_topointer(L, -1);
	lua_pop(L, 1);

	if (lua_type(L, -1) == LUA_TTABLE) {
		lua_pushvalue(L, -1);
		lua_pushstring(L, pszLuaName);
		lua_rawset(L, 1);
	}

	if (wdgWidget != NULL) {
		iCurrentArg = 0;
		switch (lua_type(L, -1)) {
		case LUA_TSTRING:
			pszName = strdup(pszLuaName);
			xmsStr = XmStringCreateLocalized(
			    (String)lua_tostring(L, -1));
			XtSetArg(aArgs[iCurrentArg], pszName, (XtArgVal)xmsStr);
			iCurrentArg++;
			break;
		case LUA_TNUMBER:
			pszName = strdup(pszLuaName);
			XtSetArg(aArgs[iCurrentArg], pszName, (XtArgVal)
			    lua_tointeger(L, -1));
			iCurrentArg++;
			break;
		case LUA_TBOOLEAN:
			pszName = strdup(pszLuaName);
			XtSetArg(aArgs[iCurrentArg], pszName, (XtArgVal)
			    (int)lua_toboolean(L, -1));
			iCurrentArg++;
			break;
		case LUA_TFUNCTION:
			/* XXX maybe leaks memory */
			pcbdCallback = GC_malloc(sizeof(struct cb_data));
			if (pcbdCallback == NULL)
				luaL_error(L, "memory error");
			pcbdCallback->L = L;
			lua_pushvalue(L, -1);
			pcbdCallback->ref = luaL_ref(L, LUA_REGISTRYINDEX);
			lua_pushvalue(L, -3);
			pcbdCallback->obj = luaL_ref(L, LUA_REGISTRYINDEX);
			pcbdCallback->callback_name = strdup(pszLuaName);
			XtAddCallback(wdgWidget, pszLuaName, lm_Callback, pcbdCallback);
			XtAddCallback(wdgWidget, XmNdestroyCallback,
			    lm_DestroyCallback, pcbdCallback);
			break;
		}
		if (iCurrentArg > 0) {
			XtSetValues(wdgWidget, aArgs, iCurrentArg);
			for (iter = 0; iter < iCurrentArg; iter++)
				free(aArgs[iter].name);
		}
	}
	return 0;
}



/* register functions with the interpereter */

int
luaopen_motif(lua_State *L)
{
	int n;

	struct luaL_Reg luamotif[] = {
		{ "AddInput",			lm_AddInput },
		{ "Realize",			lm_NewRealize },
		{ "Unrealize",			lm_Unrealize },
		{ "Initialize",			lm_Initialize },
		{ "RemoveInput",		lm_RemoveInput },
		{ "RemoveTimeOut",		lm_RemoveTimeOut },
		{ "SetLanguageProc",		lm_XtSetLanguageProc },
		{ NULL, NULL }
	};
	struct luaL_Reg luaapp[] = {
		{ "__index",			lm_index },
		{ "__newindex",			lm_newindex },

		{ "CreateInformationDialog",	lm_CreateInformationDialog },
		{ "CreateFileSelectionDialog",	lm_CreateFileSelectionDialog },
		{ "CreateFormDialog",		lm_CreateFormDialog },
		{ "FileSelectionBoxGetChild",	lm_XmFileSelectionBoxGetChild },
		{ "FileSelectionDoSearch",	lm_XmFileSelectionDoSearch },
		{ "MessageBoxGetChild",		lm_XmMessageBoxGetChild },

		{ "GetPixmap",			lm_GetPixmap },
		{ "DestroyPixmap",		lm_DestroyPixmap },

		/* Lists */
		{ "ListAddItem",		lm_XmListAddItem },

		/* Managing, Xt */
		{ "SetSensitive",		lm_SetSensitive },
		{ "ManageChild",		lm_ManageChild },
		{ "UnmanageChild",		lm_UnmanageChild },
		{ "Screen",			lm_Screen },
		{ "Window",			lm_Window },
		{ "Parent",			lm_Parent },
		{ "XtParent",			lm_XtParent },
		{ "Widget",			lm_Widget },
		{ "Popup",			lm_Popup },
		{ "Popdown",			lm_Popdown },
		{ "UpdateDisplay",		lm_UpdateDisplay },
		{ "ProcessTraversal",		lm_ProcessTraversal },
		{ "AddCallback",		lm_AddCallback },
		{ "Destroy",			lm_DestroyWidgetHierarchy },
		{ "GetValues",			lm_GetValues },
		{ "SetValues",			lm_SetValues },
		{ "SetStringValue",		lm_SetStringValue },
		{ "SetKeyboardFocus",		lm_SetKeyboardFocus },
		{ "SetWorkWindow",		lm_SetWorkWindow },
		{ "ScrollVisible",		lm_ScrollVisible },

		/* XmText */
		{ "GetString",			lm_GetString },
		{ "SetString",			lm_SetString },
		{ "Remove",			lm_Remove },
		{ "Replace",			lm_Replace },
		{ "Insert",			lm_Insert },
		{ "GetInsertionPosition",	lm_GetInsertionPosition },
		{ "GetLastPosition",		lm_GetLastPosition },
		{ "SetInsertionPosition",	lm_SetInsertionPosition },
		{ "SetMaxLength",		lm_SetMaxLength },
		{ "SetSelection",		lm_SetSelection },
		{ NULL,				NULL }
	};
	struct luaL_Reg luaXtApp[] = {
		{ "AddInput",			lm_AddInput },
		{ "RemoveInput",		lm_RemoveInput },
		{ "AddTimeOut",			lm_AddTimeOut },
		{ "RemoveTimeOut",		lm_RemoveTimeOut },
		{ "MainLoop",			lm_MainLoop },
		{ "SetExitFlag",		lm_SetExitFlag },
		{ "ProcessEvent",		lm_ProcessEvent },
		{ NULL,				NULL }
	};
	luaL_newlib(L, luamotif);
	lm_set_info(L);
	lm_register(L, lm_widgetConstructors);
	//lm_register(L, lm_gadgetConstructors);

	for (n = 0; n < num_motif_strings(); n++) {
		lua_pushstring(L, motif_strings[n].value);
		lua_setfield(L, -2, motif_strings[n].name);
	};
	for (n = 0; n < num_motif_ints(); n++) {
		lua_pushinteger(L, motif_ints[n].value);
		lua_setfield(L, -2, motif_ints[n].name);
	};

	/* The Widget metatable */
	if (luaL_newmetatable(L, WIDGET_METATABLE)) {
		luaL_setfuncs(L, luaapp, 0);
		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);

	/* The Xt application context metatable */
	if (luaL_newmetatable(L, CONTEXT_METATABLE)) {
		luaL_setfuncs(L, luaXtApp, 0);
		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);

	return 1;
}
