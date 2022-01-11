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
#include "include/callbacks.h"



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