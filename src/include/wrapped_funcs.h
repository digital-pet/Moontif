#pragma once

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

int lm_XmFileSelectionBoxGetChild(lua_State* L);

int lm_XmFileSelectionDoSearch(lua_State* L);

int lm_XmMessageBoxGetChild(lua_State* L);

int lm_XmListAddItem(lua_State* L);

int lm_ProcessTraversal(lua_State* L);

int lm_GetString(lua_State* L);

int lm_Remove(lua_State* L);

int lm_Replace(lua_State* L);

int lm_Insert(lua_State* L);

int lm_GetInsertionPosition(lua_State* L);

int lm_GetLastPosition(lua_State* L);

int lm_SetInsertionPosition(lua_State* L);

int lm_SetMaxLength(lua_State* L);

int lm_SetSelection(lua_State* L);

int lm_SetString(lua_State* L);

int lm_UpdateDisplay(lua_State* L);

int lm_GetValues(lua_State* L);

int lm_SetValues(lua_State* L);

int lm_SetStringValue(lua_State* L);

int lm_SetKeyboardFocus(lua_State* L);

int lm_SetWorkWindow(lua_State* L);

int lm_ScrollVisible(lua_State* L);

int lm_XtParent(lua_State* L);

int lm_ManageChild(lua_State* L);

int lm_UnmanageChild(lua_State* L);

int lm_SetSensitive(lua_State* L);

int lm_XtSetLanguageProc(lua_State* L);

int lm_Window(lua_State* L);

int lm_Screen(lua_State* L);

int lm_Popup(lua_State* L);

int lm_Popdown(lua_State* L);

int lm_MainLoop(lua_State* L);

int lm_SetExitFlag(lua_State* L);

int lm_ProcessEvent(lua_State* l);

int lm_Unrealize(lua_State* L);

int lm_GetPixmap(lua_State* L);

int lm_DestroyPixmap(lua_State* L);

int lm_Realize(lua_State* L);