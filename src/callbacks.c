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

#include "include/common.h"
#include "include/luamotif.h"
#include "include/wrapped_funcs.h"

void lm_Callback(Widget widget, XtPointer client_data, XtPointer call_data)
{
	struct cb_data* cbdCallback = (struct cb_data*)client_data;
	int iArgCount = 0;

	lua_rawgeti(cbdCallback->L, LUA_REGISTRYINDEX, cbdCallback->ref);
	if (cbdCallback->obj != LUA_NOREF) {
		lua_rawgeti(cbdCallback->L, LUA_REGISTRYINDEX, cbdCallback->obj);
		iArgCount = 1;
	};

	if (!strcmp(cbdCallback->callback_name, XmNtabSelectedCallback)) {
		XmTabStackCallbackStruct* ts_cbd =
			(XmTabStackCallbackStruct*)call_data;

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

void lm_DestroyCallback(Widget widget, XtPointer client_data, XtPointer call_data)
{
	struct cb_data* cbdCallback = (struct cb_data*)client_data;

	luaL_unref(cbdCallback->L, LUA_REGISTRYINDEX, cbdCallback->ref);
	luaL_unref(cbdCallback->L, LUA_REGISTRYINDEX, cbdCallback->obj);
	GC_free(cbdCallback->callback_name);
	GC_free(cbdCallback);
}

int lm_AddCallback(lua_State* L)
{
	Widget wdgWidget;
	const char* pszCallbackName;
	struct cb_data* pcbdCallback;

	wdgWidget = lm_GetWidget(L, 1);
	if (!lua_isstring(L, 2))
		luaL_argerror(L, 2, "string expected");
	if (!lua_isfunction(L, 3))
		luaL_argerror(L, 3, "function expected");

	pcbdCallback = GC_malloc(sizeof(struct cb_data));
	if (pcbdCallback == NULL)
		luaL_error(L, "memory error");

	pcbdCallback->L = L;

	/* reference the function */
	pcbdCallback->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	pszCallbackName = lua_tostring(L, -1);
	pcbdCallback->callback_name = strdup(pszCallbackName);
	XtAddCallback(wdgWidget, (char*)pszCallbackName, lm_Callback, pcbdCallback);
	lua_pop(L, 1);
	/* reference the widget */
	pcbdCallback->obj = luaL_ref(L, LUA_REGISTRYINDEX);
	XtAddCallback(wdgWidget, XmNdestroyCallback, lm_DestroyCallback, pcbdCallback);
	return 0;
}