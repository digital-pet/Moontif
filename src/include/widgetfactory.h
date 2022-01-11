#pragma once

#include <X11/Xdefs.h>
#include <Xm/XmAll.h>

typedef Widget(*WidgetFac1)(Widget, String, ArgList, Cardinal);

typedef Widget(*WidgetFactory)(lua_State*, int, Widget, char*);

Widget ConstructGenericWidget(lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName, WidgetFac1 WidgetFunc);