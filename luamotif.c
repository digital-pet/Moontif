/*
 * This file is part of the luamotif-core distribution (https://github.com/digital-pet/luamotif-core).
 * Portions Copyright (c) 2022 digital-pet.
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
 * Copyright (c) 2009 - 2018, Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
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

/* Lua binding for Motif */

#include <stdlib.h>
#ifdef __linux__
#include <bsd/string.h>
#endif

#include <X11/Xdefs.h>
#include <Xm/XmAll.h>

#include <iconv.h>
#include <lua.h>
#include <lauxlib.h>

#include "luamotif.h"
#include "dialog.h"
#include "callbacks.h"
#include "wrapped_funcs.h"

extern struct luaL_Reg lm_gadgetConstructors[];
extern struct luaL_Reg lm_widgetConstructors[];

extern struct str_constant motif_strings[];
extern struct int_constant motif_ints[];
extern size_t num_motif_strings(void);
extern size_t num_motif_ints(void);

static struct str_constant *typestr;

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
		argv = malloc(sizeof(String));
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



static void lm_Input(XtPointer client_data, int *source, XtInputId *id)
{
	lm_callbackdata *cbd = (lm_callbackdata *)client_data;

	lua_rawgeti(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	if (lua_pcall(cbd->L, 0, 0, 0))
		luaL_error(cbd->L, "error calling input function:\n%s",
		    lua_tostring(cbd->L, -1));
	/*luaL_unref(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	free(cbd);*/
}

static int lm_AddInput(lua_State *L)
{
	XtAppContext *app;
	XtIntervalId id;
	lm_callbackdata *cbd;
	int fd;

	app = luaL_checkudata(L, 1, CONTEXT_METATABLE);

	cbd = malloc(sizeof(lm_callbackdata));
	cbd->L = L;
	cbd->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	fd = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	id = XtAppAddInput(*app, fd, (XtPointer)XtInputReadMask, lm_Input, cbd);
	lua_pushinteger(L, id);
	return 1;
}

static int lm_RemoveInput(lua_State *L)
{
	XtRemoveInput(lua_tointeger(L, 1));
	return 0;
}

static void lm_Interval(XtPointer client_data, XtIntervalId *ignored)
{
	lm_callbackdata *cbd = (lm_callbackdata *)client_data;

	lua_rawgeti(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	if (lua_pcall(cbd->L, 0, 0, 0))
		luaL_error(cbd->L, "error calling timeout function:\n%s",
		    lua_tostring(cbd->L, -1));
	luaL_unref(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	free(cbd);
}

static int lm_AddTimeOut(lua_State *L)
{
	XtAppContext *app;
	XtIntervalId id;
	lm_callbackdata *cbd;
	unsigned long interval;

	app = luaL_checkudata(L, 1, CONTEXT_METATABLE);

	cbd = malloc(sizeof(lm_callbackdata));
	cbd->L = L;
	cbd->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	interval = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	id = XtAppAddTimeOut(*app, interval, lm_Interval, cbd);
	lua_pushinteger(L, id);
	return 1;
}

static int lm_RemoveTimeOut(lua_State *L)
{
	XtRemoveTimeOut(luaL_checkinteger(L, 1));
	return 0;
}

static int
type_compare(const void *r1, const void *r2)
{
	struct str_constant *s1, *s2;

	s1 = (struct str_constant *)r1;
	s2 = (struct str_constant *)r2;

	return strcmp(s1->value, s2->value);
}

int get_type(const char *string)
{
	struct str_constant *constant, key;
	int type = LUA_TNIL;

	if (typestr == NULL) {
		typestr = motif_strings;
		qsort(&motif_strings[0],
		    num_motif_strings(),
		    sizeof(motif_strings[0]), type_compare);
	}
	key.value = string;
	constant = bsearch(&key, &motif_strings[0],
	    num_motif_strings(),
	    sizeof(motif_strings[0]), type_compare);
	if (constant)
		type = constant->type;
	return type;
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
			pcbdCallback = malloc(sizeof(struct cb_data));
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

#define MAXARGS 64

static Widget
GetTopShell(Widget wdgWidget)
{
	while (wdgWidget && !XtIsWMShell(wdgWidget))
		wdgWidget = XtParent(wdgWidget);
	return wdgWidget;
}

Widget
lm_CreateWidgetHierarchy(lua_State *L, int parentObj, Widget wdgParent, const char *pszArgumentName)
{
	Arg aArgs[MAXARGS];
	WidgetClass class;
	Widget wdgWidget;
	XmString xmsStr;
#if 0
	iconv_t cd;
#endif
	struct cb_data *cbdCallback;
#if 0
	int data = -1;
	size_t len, inbytesleft, outbytesleft;
#endif
	char szName[64], *pszName;
	int iter, iCurrentArg, iLuaTableID;
	char *pszUtf8Str;
#if 0
	char *utf8_s, *iso_s;
	char *inbuf, *outbuf;
#endif
	wdgWidget = NULL;
	iCurrentArg = 0;

	lua_pushstring(L, "__widgetClass");
	lua_rawget(L, -2);
	class = (WidgetClass)lua_topointer(L, -1);
	lua_pop(L, 1);

	if (class != NULL) {
		iLuaTableID = lua_gettop(L);
		lua_pushnil(L);
		while (lua_next(L, iLuaTableID) != 0) {
			switch (lua_type(L, -2)) {
			case LUA_TSTRING:
				strlcpy(szName, lua_tostring(L, -2), sizeof szName);
				break;
			case LUA_TNUMBER:
				snprintf(szName, sizeof szName, "%.f",
				    lua_tonumber(L, -2));
				break;
			default:
				strlcpy(szName, pszArgumentName, sizeof szName);
			}

			/* Processed after widget creation */
			if (!strcmp(szName, "background") ||
			    !strcmp(szName, "foreground")) {
				lua_pop(L, 1);
				continue;
			}

			switch (lua_type(L, -1)) {
			case LUA_TSTRING:
				pszUtf8Str = (char *)lua_tostring(L, -1);
#if 0
				len = strlen(iso_s) * 2 + 1;
				utf8_s = malloc(len);

				cd = iconv_open("UTF-8", "ISO-8859-1");
				inbytesleft = strlen(iso_s);
				outbytesleft = len;
				inbuf = iso_s;
				outbuf = utf8_s;
				bzero(utf8_s, len);
				iconv(cd, (const char**)&inbuf, &inbytesleft,
				    &outbuf, &outbytesleft);
				iconv_close(cd);
#endif
				pszName = strdup(szName);
				/* XXX ugly, ugliest... */
				if (strcmp(szName, "value") &&
				    strcmp(szName, "title")) {
					xmsStr = XmStringCreateLocalized(pszUtf8Str);
					XtSetArg(aArgs[iCurrentArg], pszName, (XtArgVal)xmsStr);
				} else
					XtSetArg(aArgs[iCurrentArg], pszName, pszUtf8Str);
				iCurrentArg++;
				break;
			case LUA_TNUMBER:
				pszName = strdup(szName);
#if LUA_VERSION_NUM >= 503
				XtSetArg(aArgs[iCurrentArg], pszName, (XtArgVal)
				    lua_tointeger(L, -1));
#else
				XtSetArg(args[narg], nm, (XtArgVal)
				    (int)lua_tonumber(L, -1));
#endif
				iCurrentArg++;
				break;
			case LUA_TBOOLEAN:
				pszName = strdup(szName);
				XtSetArg(aArgs[iCurrentArg], pszName, (XtArgVal)
				    (int)lua_toboolean(L, -1));
				iCurrentArg++;
				break;
			}
			lua_pop(L, 1);
		}
	}

	if (class == xmDialogShellWidgetClass)
		wdgWidget = XtCreatePopupShell(pszArgumentName, class, GetTopShell(wdgParent),
		    aArgs, iCurrentArg);
	else if (class == xmBulletinBoardWidgetClass)
		wdgWidget = XmCreateBulletinBoard(wdgParent, (String)pszArgumentName, aArgs, iCurrentArg);
	else if (class != NULL)
		wdgWidget = XtCreateWidget(pszArgumentName, class, wdgParent, aArgs, iCurrentArg);
	if (wdgWidget != NULL) {
		lua_pushlightuserdata(L, wdgWidget);
		lua_setfield(L, -2, "__widget");
		luaL_getmetatable(L, WIDGET_METATABLE);
		lua_setmetatable(L, -2);
	}

	iLuaTableID = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, iLuaTableID) != 0) {
		switch (lua_type(L, -2)) {
		case LUA_TSTRING:
			strlcpy(szName, lua_tostring(L, -2), sizeof szName);
			break;
		case LUA_TNUMBER:
			snprintf(szName, sizeof iter, "%.f", lua_tonumber(L, -2));
			break;
		default:
			strlcpy(szName, pszArgumentName, sizeof szName);
		}

		switch (lua_type(L, -1)) {
		case LUA_TSTRING:
			if (wdgWidget && (!strcmp(szName, "background") ||
			    !strcmp(szName, "foreground"))) {
				size_t len;
				const char *pszColor;

				pszColor = luaL_checklstring(L, -1, &len);
				XtVaSetValues(wdgWidget, XtVaTypedArg, szName,
					XmRString, pszColor, len + 1, NULL);
			}
			break;
		case LUA_TTABLE:
			if (wdgWidget == NULL)
				lm_CreateWidgetHierarchy(L, iLuaTableID, wdgParent, szName);
			else
				lm_CreateWidgetHierarchy(L, iLuaTableID, wdgWidget, szName);
			break;
		case LUA_TFUNCTION:
			if (wdgWidget == NULL)
				break;

			/* XXX maybe leaks memory */
			cbdCallback = malloc(sizeof(struct cb_data));
			if (cbdCallback == NULL)
				luaL_error(L, "memory error");
			cbdCallback->L = L;
			lua_pushvalue(L, -1);
			cbdCallback->ref = luaL_ref(L, LUA_REGISTRYINDEX);
			lua_pushvalue(L, iLuaTableID);
			cbdCallback->obj = luaL_ref(L, LUA_REGISTRYINDEX);
			cbdCallback->callback_name = strdup(szName);
			XtAddCallback(wdgWidget, szName, lm_Callback, cbdCallback);
			XtAddCallback(wdgWidget, XmNdestroyCallback,
			    lm_DestroyCallback, cbdCallback);
			break;
		}
		lua_pop(L, 1);
	}
	for (iter = 0; iter < iCurrentArg; iter++)
		free(aArgs[iter].name);
	if (parentObj > 0) {
		lua_pushstring(L, "__parent");
		lua_pushvalue(L, parentObj);
		lua_rawset(L, -3);
		/* lua_setfield(L, -2, "__parent"); */ /*lua_setfield does not work here, it assigns the value __parent to the key parentObj for some reason */
	}
	if (wdgWidget)
		XtManageChild(wdgWidget);
	return wdgWidget;
}

static int
lm_DestroyWidgetHierarchy(lua_State *L)
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



/* register functions with the interpereter */

int
luaopen_motif(lua_State *L)
{
	int n;

	struct luaL_Reg luamotif[] = {
		{ "AddInput",			lm_AddInput },
		{ "Realize",			lm_Realize },
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
		{ "AddTimeOut",			lm_AddTimeOut },
		{ "MainLoop",			lm_MainLoop },
		{ "SetExitFlag",		lm_SetExitFlag },
		{ "ProcessEvent",		lm_ProcessEvent },
		{ NULL,				NULL }
	};
	luaL_newlib(L, luamotif);
	lm_set_info(L);
	lm_register(L, lm_widgetConstructors);
	lm_register(L, lm_gadgetConstructors);

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
