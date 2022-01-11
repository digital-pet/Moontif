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

int lm_CreateFileSelectionDialog(lua_State* L)
{
	Widget wdgParent, wdgDialog;
	const char* pszName;

	wdgParent = lm_GetWidget(L, 1);

	pszName = luaL_checkstring(L, 2);
	wdgDialog = XmCreateFileSelectionDialog(wdgParent, (char*)pszName, NULL, 0);
	XtManageChild(wdgDialog);
	lua_newtable(L);
	lua_pushlightuserdata(L, wdgDialog);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);

	return 1;
}

int lm_CreateFormDialog(lua_State* L)
{
	Widget wdgParent, wdgDialog;
	const char* pszName;

	wdgParent = lm_GetWidget(L, 1);

	pszName = luaL_checkstring(L, 2);
	wdgDialog = XmCreateFormDialog(wdgParent, (char*)pszName, NULL, 0);
	XtManageChild(wdgDialog);
	lua_newtable(L);
	lua_pushlightuserdata(L, wdgDialog);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);

	return 1;
}

int lm_CreateInformationDialog(lua_State* L)
{
	Widget wdgParent, wdgDialog;
	const char* pszName;

	wdgParent = lm_GetWidget(L, 1);

	pszName = luaL_checkstring(L, 2);
	wdgDialog = XmCreateInformationDialog(wdgParent, (char*)pszName, NULL, 0);
	XtManageChild(wdgDialog);
	lua_newtable(L);
	lua_pushlightuserdata(L, wdgDialog);
	lua_setfield(L, -2, "__widget");
	luaL_getmetatable(L, WIDGET_METATABLE);
	lua_setmetatable(L, -2);

	return 1;
}
