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
 * along with this program. If not, see <http://www.gnu.org/licenses/
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

#include "include/luamotif.h"
#include "include/callbacks.h"
#include "include/widgetfactory.h"

Widget CreateManagedWidgetTree(lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName) {
	
	WidgetFactory WidgetFunc;
	Widget wdgWidget;
	Arg aArgs[MAXARGS];
	XmString* axmsValues;
	int iXmStringCount;
	struct cb_data* cbdCallback;
	char szKey[64], * pszAttributeName;
	int iter, iCurrentArg, iLuaTableID;
	char* pszUtf8Str;

	wdgWidget = NULL;
	iCurrentArg = 0;
	iXmStringCount = 0;

	/*
	 *
	 *  First pass: Gather creation-time arguments, then create widget.
	 *
	 */


	/*
	 *
	 * Second pass: Recurse.
	 *
	 */

	/*
	 *
	 * Third pass: Gather post-creation arguments, apply, then manage if not top level.
	 *
	 */

	if (parentObj > 0) {
		XtManageChild(wdgWidget);
	}

	return wdgWidget;
}