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

#include "include/common.h"

static void lm_Input(XtPointer client_data, int* source, XtInputId* id)
{
	lm_callbackdata* cbd = (lm_callbackdata*)client_data;

	lua_rawgeti(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	if (lua_pcall(cbd->L, 0, 0, 0))
		luaL_error(cbd->L, "error calling input function:\n%s",
			lua_tostring(cbd->L, -1));
	/*luaL_unref(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	free(cbd);*/
}

int lm_AddInput(lua_State* L)
{
	XtAppContext* app;
	XtIntervalId id;
	lm_callbackdata* cbd;
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

int lm_RemoveInput(lua_State* L)
{
	XtRemoveInput(lua_tointeger(L, 1));
	return 0;
}

static void lm_Interval(XtPointer client_data, XtIntervalId* ignored)
{
	lm_callbackdata* cbd = (lm_callbackdata*)client_data;

	lua_rawgeti(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	if (lua_pcall(cbd->L, 0, 0, 0))
		luaL_error(cbd->L, "error calling timeout function:\n%s",
			lua_tostring(cbd->L, -1));
	luaL_unref(cbd->L, LUA_REGISTRYINDEX, cbd->ref);
	free(cbd);
}

int lm_AddTimeOut(lua_State* L)
{
	XtAppContext* app;
	XtIntervalId id;
	lm_callbackdata* cbd;
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

int lm_RemoveTimeOut(lua_State* L)
{
	XtRemoveTimeOut(luaL_checkinteger(L, 1));
	return 0;
}

int lm_MainLoop(lua_State* L)
{
	XtAppContext* app;

	app = luaL_checkudata(L, 1, CONTEXT_METATABLE);
	XtAppMainLoop(*app);
	return 0;
}

int lm_SetExitFlag(lua_State* L)
{
	XtAppContext* app;

	app = luaL_checkudata(L, 1, CONTEXT_METATABLE);
	XtAppSetExitFlag(*app);
	return 0;
}

int lm_ProcessEvent(lua_State* l)
{
	XEvent event;
	XtAppContext* app;

	app = luaL_checkudata(l, 1, CONTEXT_METATABLE);
	XtAppNextEvent(*app, &event);
	XtDispatchEvent(&event);
	return 0;
}