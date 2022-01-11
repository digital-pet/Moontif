#pragma once

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
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */

#define GENERIC_WIDGET_CONSTRUCTOR(W) \
  static Widget _Construct##W (lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName) { \
	_lua_stackguard_entry(L); \
	Widget wdgWidget; \
	wdgWidget = ConstructGenericWidget(L, parentObj, wdgParent, pszWidgetName, XmCreate##W ); \
	_lua_stackguard_exit(L); \
	return wdgWidget; \
}

#define COMPLEX_WIDGET_CONSTRUCTOR(W) \
  static Widget _Construct##W (lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName) { \
	_lua_stackguard_entry(L); \
	Widget wdgWidget; \
	wdgWidget = Construct##W (L, parentObj, wdgParent, pszWidgetName); \
	_lua_stackguard_exit(L); \
	return wdgWidget; \
}

#define GENERIC_WIDGET_CLASS(W) \
static int lm_Defer##W (lua_State *L)\
{ \
	_lua_stackguard_entry(L); \
	lua_pushlightuserdata(L, _Construct##W ); \
	lua_setfield(L, -2, "__widgetConstructor"); \
	_lua_stackguard_exit(L); \
	return 1; \
}

#define GENERIC_WIDGET(W) \
GENERIC_WIDGET_CONSTRUCTOR(W) \
GENERIC_WIDGET_CLASS(W)

#define COMPLEX_WIDGET(W) \
COMPLEX_WIDGET_CONSTRUCTOR(W) \
GENERIC_WIDGET_CLASS(W)
