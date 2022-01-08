/*
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

static int lm_getArgs(lua_State *L, int start, Arg **args);
static int get_type(const char *);

extern struct luaL_Reg lm_gadgetConstructors[];
extern struct luaL_Reg lm_widgetConstructors[];

extern struct str_constant motif_strings[];
extern struct int_constant motif_ints[];
extern size_t num_motif_strings(void);
extern size_t num_motif_ints(void);

static struct str_constant *typestr;

static Widget
lm_GetWidget(lua_State *L, int iLuaTableID)
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

static int
lm_CreateFileSelectionDialog(lua_State *L)
{
	Widget wdgParent, wdgDialog;
	const char *pszName;

	wdgParent = lm_GetWidget(L, 1);

	pszName = luaL_checkstring(L, 2);
	wdgDialog = XmCreateFileSelectionDialog(wdgParent, (char *)pszName, NULL, 0);
	XtManageChild(wdgDialog);
	lua_newtable(L);
	lua_pushlightuserdata(L, wdgDialog);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);

	return 1;
}

static int
lm_CreateFormDialog(lua_State *L)
{
	Widget wdgParent, wdgDialog;
	const char *pszName;

	wdgParent = lm_GetWidget(L, 1);

	pszName = luaL_checkstring(L, 2);
	wdgDialog = XmCreateFormDialog(wdgParent, (char *)pszName, NULL, 0);
	XtManageChild(wdgDialog);
	lua_newtable(L);
	lua_pushlightuserdata(L, wdgDialog);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);

	return 1;
}

static int
lm_CreateInformationDialog(lua_State *L)
{
	Widget wdgParent, wdgDialog;
	const char *pszName;

	wdgParent = lm_GetWidget(L, 1);

	pszName = luaL_checkstring(L, 2);
	wdgDialog = XmCreateInformationDialog(wdgParent, (char *)pszName, NULL, 0);
	XtManageChild(wdgDialog);
	lua_newtable(L);
	lua_pushlightuserdata(L, wdgDialog);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);

	return 1;
}

static int
lm_XmFileSelectionBoxGetChild(lua_State *L)
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

static int
lm_XmFileSelectionDoSearch(lua_State *L)
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


static int
lm_XmMessageBoxGetChild(lua_State *L)
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

/* XmList */
static int
lm_XmListAddItem(lua_State *L)
{
	Widget wdgList;
	char *pszItemString;
	XmString xmsItemString;

	wdgList = lm_GetWidget(L, 1);
	pszItemString = (char *)luaL_checkstring(L, 2);
	xmsItemString = XmStringCreateLocalized(pszItemString);
	XmListAddItem(wdgList, xmsItemString, luaL_checkinteger(L, 3));
	XmStringFree(xmsItemString);
	return 0;
}

static int
lm_ProcessTraversal(lua_State *L)
{
	Widget wdgWidget;
	int iDirection;

	wdgWidget = lm_GetWidget(L, 1);
	iDirection = luaL_checkinteger(L, 2);
	XmProcessTraversal(wdgWidget, (XmTraversalDirection)iDirection);
	return 0;
}

static int
lm_GetString(lua_State *L)
{
	Widget wdgWidget;
	char *pszString;

	wdgWidget = lm_GetWidget(L, 1);

	pszString = XmTextGetString(wdgWidget);
	if (pszString == NULL)
		lua_pushnil(L);
	else
		lua_pushstring(L, pszString);
	return 1;
}

static int
lm_Remove(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextRemove(wdgWidget);
	return 0;
}

static int
lm_Replace(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextReplace(wdgWidget, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
	    (char *)luaL_checkstring(L, 4));
	return 0;
}

static int
lm_Insert(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);

	XmTextInsert(wdgWidget, luaL_checkinteger(L, 2),
	    (char *)luaL_checkstring(L, 3));
	return 0;
}

static int
lm_GetInsertionPosition(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	lua_pushinteger(L, XmTextGetInsertionPosition(wdgWidget));
	return 1;
}

static int
lm_GetLastPosition(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	lua_pushinteger(L, XmTextGetLastPosition(wdgWidget));
	return 1;
}

static int
lm_SetInsertionPosition(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextSetInsertionPosition(wdgWidget, luaL_checkinteger(L, 2));
	return 0;
}

static int
lm_SetMaxLength(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextSetMaxLength(wdgWidget, luaL_checkinteger(L, 2));
	return 0;
}

static int
lm_SetSelection(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);

	XmTextSetSelection(wdgWidget, luaL_checkinteger(L, 2),
	    luaL_checkinteger(L, 3),
	    XtLastTimestampProcessed(XtDisplay(wdgWidget)));
	return 0;
}

static int
lm_SetString(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmTextSetString(wdgWidget, (char *)luaL_checkstring(L, 2));
	return 0;
}

static int
lm_UpdateDisplay(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XmUpdateDisplay(wdgWidget);
	return 0;
}

static void
lm_Callback(Widget widget, XtPointer client_data, XtPointer call_data)
{
	struct cb_data *cbdCallback = (struct cb_data *)client_data;
	int iArgCount = 0;

	lua_rawgeti(cbdCallback->L, LUA_REGISTRYINDEX, cbdCallback->ref);
	if (cbdCallback->obj != LUA_NOREF) {
		lua_rawgeti(cbdCallback->L, LUA_REGISTRYINDEX, cbdCallback->obj);
		iArgCount = 1;
	};

	if (!strcmp(cbdCallback->callback_name, XmNtabSelectedCallback)) {
		XmTabStackCallbackStruct * ts_cbd =
		    (XmTabStackCallbackStruct *)call_data;

		lua_newtable(cbdCallback->L);
		lua_pushlightuserdata(cbdCallback->L, ts_cbd->selected_child);
		lua_setfield(cbdCallback->L, -2, "__widget");
		luaL_getmetatable(cbdCallback->L, WIDGET_METATABLE);
		lua_setmetatable(cbdCallback->L, -2);
		iArgCount++;
	}

	if (lua_pcall(cbdCallback->L, iArgCount, 0, 0))
		luaL_error(cbdCallback->L, "error calling callback function:\n%s",
		    lua_tostring(cbdCallback->L, -1));
}

static void
lm_DestroyCallback(Widget widget, XtPointer client_data, XtPointer call_data)
{
	struct cb_data *cbdCallback = (struct cb_data *)client_data;

	luaL_unref(cbdCallback->L, LUA_REGISTRYINDEX, cbdCallback->ref);
	luaL_unref(cbdCallback->L, LUA_REGISTRYINDEX, cbdCallback->obj);
	free(cbdCallback->callback_name);
	free(cbdCallback);
}

static int
lm_AddCallback(lua_State *L)
{
	Widget wdgWidget;
	const char *pszCallbackName;
	struct cb_data *pcbdCallback;

	wdgWidget = lm_GetWidget(L, 1);
	if (!lua_isstring(L, 2))
		luaL_argerror(L, 2, "string expected");
	if (!lua_isfunction(L, 3))
		luaL_argerror(L, 3, "function expected");

	/* XXX maybe leaks memory (e.g. when the widget is destroyed */
	pcbdCallback = malloc(sizeof(struct cb_data));
	if (pcbdCallback == NULL)
		luaL_error(L, "memory error");

	pcbdCallback->L = L;

	/* reference the function */
	pcbdCallback->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	pszCallbackName = lua_tostring(L, -1);
	pcbdCallback->callback_name = strdup(pszCallbackName);
	XtAddCallback(wdgWidget, (char *)pszCallbackName, lm_Callback, pcbdCallback);
	lua_pop(L, 1);
	/* reference the widget */
	pcbdCallback->obj = luaL_ref(L, LUA_REGISTRYINDEX);
	XtAddCallback(wdgWidget, XmNdestroyCallback, lm_DestroyCallback, pcbdCallback);
	return 0;
}

static int
lm_getArgs(lua_State *L, int iStartPosition, Arg **args)
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

static int
lm_GetValues(lua_State *L)
{
	Arg args[1];
	Widget wdgWidget;
	int iCurrentArgument, iter, iterMax, iArgType;
	const char *pszArgName;
	char *pszArgValue;
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

static int
lm_SetValues(lua_State *L)
{
	Arg *args;
	Widget widget;
	int narg;

	widget = lm_GetWidget(L, 1);
	narg = lm_getArgs(L, 2, &args);
	XtSetValues(widget, args, narg);
	free(args);
	return 0;
}

static int
lm_SetStringValue(lua_State *L)
{
	Widget widget;
	size_t len;
	const char *name, *value;

	name = luaL_checkstring(L, 2);
	value = luaL_checklstring(L, 3, &len);
	widget = lm_GetWidget(L, 1);
	XtVaSetValues(widget, XtVaTypedArg, name, XmRString, value, len + 1,
	    NULL);
	return 0;
}

static int
lm_SetKeyboardFocus(lua_State *L)
{
	Widget subtree, descendant;

	subtree = lm_GetWidget(L, 1);
	descendant = lm_GetWidget(L, 2);
	XtSetKeyboardFocus(subtree, descendant);
	return 0;
}

static int
lm_SetWorkWindow(lua_State *L)
{
	Widget widget, work_window;

	widget = lm_GetWidget(L, 1);
	work_window = lm_GetWidget(L, 2);
	XtVaSetValues(widget, XmNworkWindow, work_window, NULL);
	return 0;
}

static int
lm_ScrollVisible(lua_State *L)
{
	Widget w, scroll_to;

	w = lm_GetWidget(L, 1);
	scroll_to = lm_GetWidget(L, 2);

	XmScrollVisible(w, scroll_to, 0, 0);
	return 0;
}

/* Return the true widget */
static int
lm_Widget(lua_State *L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);

	lua_pushlightuserdata(L, widget);

	return 1;
}

/* Return the true XtParent as a widget */
static int
lm_XtParent(lua_State *L)
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

/* Return the Parent in the Lua object hierarchy */
static int
lm_Parent(lua_State *L)
{
	if (!lua_istable(L, -1))
		luaL_argerror(L, -1, "table expected");

	lua_pushstring(L, "__parent");
	lua_rawget(L, -2);
	return 1;
}

static int
lm_Popdown(lua_State *L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);
	XtPopdown(widget);
	return 0;
}

static int
lm_Popup(lua_State *L)
{
	Widget widget;
	int grab;

	widget = lm_GetWidget(L, 1);
	grab = luaL_checkinteger(L, 2);
	XtPopup(widget, grab);
	return 0;
}

static int
lm_Screen(lua_State *L)
{
	Widget widget;
	Screen *screen;

	widget = lm_GetWidget(L, 1);
	screen = XtScreen(widget);
	lua_pushinteger(L, screen->width);
	lua_pushinteger(L, screen->height);
	return 2;
};

static int
lm_Window(lua_State *L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);
	lua_pushinteger(L, XtWindow(widget));
	return 1;
};

static int
lm_XtSetLanguageProc(lua_State *L)
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

static int
lm_SetSensitive(lua_State *L)
{
	Widget widget;
	int value;

	widget = lm_GetWidget(L, 1);
	value = luaL_checkinteger(L, 2);
	XtSetSensitive(widget, value);
	return 0;
}

static int
lm_UnmanageChild(lua_State *L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);
	XtUnmanageChild(widget);
	return 0;
}

static int
lm_ManageChild(lua_State *L)
{
	Widget widget;

	widget = lm_GetWidget(L, 1);
	XtManageChild(widget);
	return 0;
}

static int
lm_Initialize(lua_State *L)
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

static int
lm_MainLoop(lua_State *L)
{
	XtAppContext *app;

	app = luaL_checkudata(L, 1, CONTEXT_METATABLE);
	XtAppMainLoop(*app);
	return 0;
}

static int
lm_SetExitFlag(lua_State *L)
{
	XtAppContext *app;

	app = luaL_checkudata(L, 1, CONTEXT_METATABLE);
	XtAppSetExitFlag(*app);
	return 0;
}

static int
lm_ProcessEvent(lua_State *l)
{
	XEvent event;
	XtAppContext *app;

	app = luaL_checkudata(l, 1, CONTEXT_METATABLE);
	XtAppNextEvent(*app, &event);
	XtDispatchEvent(&event);
	return 0;
}

static void
lm_Input(XtPointer client_data, int *source, XtInputId *id)
{
	lm_callbackdata *cbd = (lm_callbackdata *)client_data;

	lua_rawgeti(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	if (lua_pcall(cbd->L, 0, 0, 0))
		luaL_error(cbd->L, "error calling input function:\n%s",
		    lua_tostring(cbd->L, -1));
	/*luaL_unref(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	free(cbd);*/
}

static int
lm_AddInput(lua_State *L)
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

static int
lm_RemoveInput(lua_State *L)
{
	XtRemoveInput(lua_tointeger(L, 1));
	return 0;
}

static void
lm_Interval(XtPointer client_data, XtIntervalId *ignored)
{
	lm_callbackdata *cbd = (lm_callbackdata *)client_data;

	lua_rawgeti(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	if (lua_pcall(cbd->L, 0, 0, 0))
		luaL_error(cbd->L, "error calling timeout function:\n%s",
		    lua_tostring(cbd->L, -1));
	luaL_unref(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	free(cbd);
}

static int
lm_AddTimeOut(lua_State *L)
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

static int
lm_RemoveTimeOut(lua_State *L)
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

static int
get_type(const char *string)
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

static void
lm_set_info(lua_State *L) {
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2009 - 2018 micro systems "
	    "marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "Motif binding for Lua");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "Motif 1.3.0");
	lua_settable(L, -3);
}

static void
lm_register(lua_State *L, struct luaL_Reg *reg)
{
	while (reg->name != NULL) {
		lua_pushcfunction(L, reg->func);
		lua_setfield(L, -2, reg->name);
		reg++;
	}
}

static int
lm_index(lua_State *L)
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

static int
lm_newindex(lua_State *L)
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

static Widget
lm_CreateWidgetHierarchy(lua_State *L, int parentObj, Widget wdgParent,
    const char *pszArgumentName)
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

static int
lm_Realize(lua_State *L)
{
	Widget wdgToplevel;
	char szName[64];
	void *pTable;
	int iLuaTableID;

	wdgToplevel = lm_GetWidget(L, 1);
	if (!lua_istable(L, 2))
		luaL_argerror(L, 2, "table expected");

	if (lua_gettop(L) == 3) {
		if (!lua_isstring(L, 3))
			luaL_argerror(L, 3, "string expected");
		strlcpy(szName, lua_tostring(L, 3), sizeof szName);
		lua_pop(L, 1);
	} else {
		pTable = (void *)lua_topointer(L, 2);
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

static int
lm_Unrealize(lua_State *L)
{
	Widget wdgWidget;

	wdgWidget = lm_GetWidget(L, 1);
	XtUnrealizeWidget(wdgWidget);

	return 0;
}

static int
lm_GetPixmap(lua_State *L)
{
	Widget wdgWidget, wdgToplevel;
	Pixmap pixmap;

	wdgWidget = lm_GetWidget(L, 1);
	wdgToplevel = lm_GetWidget(L, 2);

	XtVaGetValues(wdgWidget, XmNlabelPixmap, &pixmap, NULL);
	if (pixmap != XmUNSPECIFIED_PIXMAP)
		XmDestroyPixmap(XtScreen(wdgToplevel), pixmap);

	pixmap = XmGetPixmap(XtScreen(wdgToplevel), (char *)luaL_checkstring(L, 3),
	    XmUNSPECIFIED_PIXEL, XmUNSPECIFIED_PIXEL);

	XtVaSetValues(wdgWidget, XmNlabelPixmap, pixmap, NULL);

	return 0;
}

static int
lm_DestroyPixmap(lua_State *L)
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
