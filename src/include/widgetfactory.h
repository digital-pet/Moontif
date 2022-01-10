#pragma once

#include <X11/Xdefs.h>
#include <Xm/XmAll.h>

typedef Widget(*WidgetFactory)(Widget, String, ArgList, Cardinal);