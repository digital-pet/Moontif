#pragma once

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

#define WIDGET_METATABLE "Motif widget functions"
#define CONTEXT_METATABLE "Xt application context"

typedef enum {
	NONE = 0,
	FUNCTION,
	UCHAR,
	BOOLEAN,
	DIMENSION,
	CARDINAL,
	POSITION,
	XTARGVAL,
	STRING
} x11_types;

#define X11_DIMENSION	0
#define X11_CARDINAL	1
#define X11_POSITION	2

typedef struct cb_data {
	lua_State *L;
	int ref;	/* The function to be called */
	int obj;	/* The Lua object to pass, usually the widget */
	char *callback_name;
} lm_callbackdata;

struct str_constant {
	const char *name;
	const char *value;
	int type;
};

struct int_constant {
	char *name;
	long value;
};

Widget lm_GetWidget(lua_State* L, int iLuaTableID);

int lm_getArgs(lua_State* L, int iStartPosition, Arg** args);

int get_type(const char*);

Widget lm_CreateWidgetHierarchy(lua_State* L, int parentObj, Widget wdgParent, const char* pszArgumentName);
