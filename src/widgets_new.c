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
 */

#include <stdlib.h>

#include <X11/Xdefs.h>
#include <Xm/XmAll.h>

#include <gc.h>

#include <lua.h>
#include <lauxlib.h>

#include "include/common.h"

#include "include/widgetfactory.h"

#define WIDGET_CONSTRUCTOR(W) \
  Widget Construct##W##(lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName) { \
	_lua_stackguard_entry(L); \
	Widget wdgWidget; \
	wdgWidget = ConstructGenericWidget(L, parentObj, wdgParent, pszWidgetName, XmCreate##W##); \
	_lua_stackguard_exit(L); \
	return wdgWidget; \
}

#define WIDGET_DEFERRED(W) \
static int lm_Defer##W##(lua_State *L)\
{ \
	_lua_stackguard_entry(L); \
	lua_pushlightuserdata(L, Construct##W##); \
	lua_setfield(L, -2, "__widgetConstructor"); \
	lua_pushstring(L, #W); \
	lua_setfield(L, -2, "__widgetKind");\
	_lua_stackguard_exit(L); \
	return 1; \
}

/*
 *
 *  CORE GADGETS
 *
 */

WIDGET_CONSTRUCTOR(ArrowButtonGadget)
WIDGET_DEFERRED(ArrowButtonGadget)

WIDGET_CONSTRUCTOR(LabelGadget)
WIDGET_DEFERRED(LabelGadget)

WIDGET_CONSTRUCTOR(PushButtonGadget)
WIDGET_DEFERRED(PushButtonGadget)

WIDGET_CONSTRUCTOR(SeparatorGadget)
WIDGET_DEFERRED(SeparatorGadget)

WIDGET_CONSTRUCTOR(ToggleButtonGadget)
WIDGET_DEFERRED(ToggleButtonGadget)

WIDGET_CONSTRUCTOR(ArrowButton)
WIDGET_DEFERRED(ArrowButton)

/*
 *
 *  CORE WIDGETS
 *
 */

WIDGET_CONSTRUCTOR(BulletinBoard)
WIDGET_DEFERRED(BulletinBoard)

//WIDGET_CONSTRUCTOR(ButtonBox)
//WIDGET_DEFERRED(ButtonBox)

WIDGET_CONSTRUCTOR(CascadeButton)
WIDGET_DEFERRED(CascadeButton)

WIDGET_CONSTRUCTOR(Column)
WIDGET_DEFERRED(Column)

WIDGET_CONSTRUCTOR(ComboBox)
WIDGET_DEFERRED(ComboBox)

WIDGET_CONSTRUCTOR(Command)
WIDGET_DEFERRED(Command)

WIDGET_CONSTRUCTOR(Container)
WIDGET_DEFERRED(Container)

WIDGET_CONSTRUCTOR(DataField)
WIDGET_DEFERRED(DataField)

WIDGET_CONSTRUCTOR(DialogShell)
WIDGET_DEFERRED(DialogShell)

WIDGET_CONSTRUCTOR(DrawingArea)
WIDGET_DEFERRED(DrawingArea)

WIDGET_CONSTRUCTOR(DrawnButton)
WIDGET_DEFERRED(DrawnButton)

WIDGET_CONSTRUCTOR(FileSelectionBox)
WIDGET_DEFERRED(FileSelectionBox)

//WIDGET_CONSTRUCTOR(FontSelector)
//WIDGET_DEFERRED(FontSelector)

WIDGET_CONSTRUCTOR(Form)
WIDGET_DEFERRED(Form)

WIDGET_CONSTRUCTOR(Frame)
WIDGET_DEFERRED(Frame)

//WIDGET_CONSTRUCTOR(Hierarchy)
//WIDGET_DEFERRED(Hierarchy)

//WIDGET_CONSTRUCTOR(IconBox)
//WIDGET_DEFERRED(IconBox)

WIDGET_CONSTRUCTOR(Label)
WIDGET_DEFERRED(Label)

WIDGET_CONSTRUCTOR(List)
WIDGET_DEFERRED(List)

WIDGET_CONSTRUCTOR(MainWindow)
WIDGET_DEFERRED(MainWindow)

WIDGET_CONSTRUCTOR(MessageBox)
WIDGET_DEFERRED(MessageBox)

WIDGET_CONSTRUCTOR(Notebook)
WIDGET_DEFERRED(Notebook)

//WIDGET_CONSTRUCTOR(Outline)
//WIDGET_DEFERRED(Outline)

WIDGET_CONSTRUCTOR(PanedWindow)
WIDGET_DEFERRED(PanedWindow)

WIDGET_CONSTRUCTOR(PushButton)
WIDGET_DEFERRED(PushButton)

WIDGET_CONSTRUCTOR(RowColumn)
WIDGET_DEFERRED(RowColumn)

WIDGET_CONSTRUCTOR(Scale)
WIDGET_DEFERRED(Scale)

WIDGET_CONSTRUCTOR(ScrollBar)
WIDGET_DEFERRED(ScrollBar)

WIDGET_CONSTRUCTOR(ScrolledWindow)
WIDGET_DEFERRED(ScrolledWindow)

WIDGET_CONSTRUCTOR(SelectionBox)
WIDGET_DEFERRED(SelectionBox)

WIDGET_CONSTRUCTOR(Separator)
WIDGET_DEFERRED(Separator)

WIDGET_CONSTRUCTOR(SpinBox)
WIDGET_DEFERRED(SpinBox)

WIDGET_CONSTRUCTOR(TabBox)
WIDGET_DEFERRED(TabBox)

WIDGET_CONSTRUCTOR(TabStack)
WIDGET_DEFERRED(TabStack)

WIDGET_CONSTRUCTOR(Text)
WIDGET_DEFERRED(Text)

WIDGET_CONSTRUCTOR(TextField)
WIDGET_DEFERRED(TextField)

WIDGET_CONSTRUCTOR(ToggleButton)
WIDGET_DEFERRED(ToggleButton)

//WIDGET_CONSTRUCTOR(Tree)
//WIDGET_DEFERRED(Tree)

/*
 *
 *  META WIDGETS
 *
 */

WIDGET_CONSTRUCTOR(BulletinBoardDialog)
WIDGET_DEFERRED(BulletinBoardDialog)

WIDGET_CONSTRUCTOR(ErrorDialog)
WIDGET_DEFERRED(ErrorDialog)

WIDGET_CONSTRUCTOR(FileSelectionDialog)
WIDGET_DEFERRED(FileSelectionDialog)

WIDGET_CONSTRUCTOR(FormDialog)
WIDGET_DEFERRED(FormDialog)

WIDGET_CONSTRUCTOR(InformationDialog)
WIDGET_DEFERRED(InformationDialog)

WIDGET_CONSTRUCTOR(MenuBar)
WIDGET_DEFERRED(MenuBar)

WIDGET_CONSTRUCTOR(MessageDialog)
WIDGET_DEFERRED(MessageDialog)

WIDGET_CONSTRUCTOR(OptionMenu)
WIDGET_DEFERRED(OptionMenu)

WIDGET_CONSTRUCTOR(PopupMenu)
WIDGET_DEFERRED(PopupMenu)

WIDGET_CONSTRUCTOR(PromptDialog)
WIDGET_DEFERRED(PromptDialog)

WIDGET_CONSTRUCTOR(PulldownMenu)
WIDGET_DEFERRED(PulldownMenu)

WIDGET_CONSTRUCTOR(QuestionDialog)
WIDGET_DEFERRED(QuestionDialog)

WIDGET_CONSTRUCTOR(RadioBox)
WIDGET_DEFERRED(RadioBox)

WIDGET_CONSTRUCTOR(ScrolledList)
WIDGET_DEFERRED(ScrolledList)

WIDGET_CONSTRUCTOR(ScrolledText)
WIDGET_DEFERRED(ScrolledText)

WIDGET_CONSTRUCTOR(SelectionDialog)
WIDGET_DEFERRED(SelectionDialog)

WIDGET_CONSTRUCTOR(SimpleCheckBox)
WIDGET_DEFERRED(SimpleCheckBox)

WIDGET_CONSTRUCTOR(SimpleMenuBar)
WIDGET_DEFERRED(SimpleMenuBar)

WIDGET_CONSTRUCTOR(SimpleOptionMenu)
WIDGET_DEFERRED(SimpleOptionMenu)

WIDGET_CONSTRUCTOR(SimplePopupMenu)
WIDGET_DEFERRED(SimplePopupMenu)

WIDGET_CONSTRUCTOR(SimplePulldownMenu)
WIDGET_DEFERRED(SimplePulldownMenu)

WIDGET_CONSTRUCTOR(SimpleRadioBox)
WIDGET_DEFERRED(SimpleRadioBox)

WIDGET_CONSTRUCTOR(WarningDialog)
WIDGET_DEFERRED(WarningDialog)

WIDGET_CONSTRUCTOR(WorkArea)
WIDGET_DEFERRED(WorkArea)

WIDGET_CONSTRUCTOR(WorkingDialog)
WIDGET_DEFERRED(WorkingDialog)

struct luaL_Reg lm_widgetConstructors[] = {
	{ "ArrowButtonGadget", lm_DeferArrowButtonGadget },
	{ "LabelGadget", lm_DeferLabelGadget },
	{ "PushButtonGadget", lm_DeferPushButtonGadget },
	{ "SeparatorGadget", lm_DeferSeparatorGadget },
	{ "ToggleButtonGadget", lm_DeferToggleButtonGadget },

	{ "ArrowButton", lm_DeferArrowButton },
	{ "BulletinBoard", lm_DeferBulletinBoard },
//	{ "ButtonBox", lm_DeferButtonBox },
	{ "CascadeButton", lm_DeferCascadeButton },
	{ "Column", lm_DeferColumn },
	{ "ComboBox", lm_DeferComboBox },
	{ "Command", lm_DeferCommand },
	{ "Container", lm_DeferContainer },
	{ "DataField", lm_DeferDataField },
	{ "DialogShell", lm_DeferDialogShell },
	{ "DrawingArea", lm_DeferDrawingArea },
	{ "DrawnButton", lm_DeferDrawnButton },
	{ "FileSelectionBox", lm_DeferFileSelectionBox },
//	{ "FontSelector", lm_DeferFontSelector },
	{ "Form", lm_DeferForm },
	{ "Frame", lm_DeferFrame },
//	{ "Hierarchy", lm_DeferHierarchy },
//	{ "IconBox", lm_DeferIconBox },
	{ "Label", lm_DeferLabel },
	{ "List", lm_DeferList },
	{ "MainWindow", lm_DeferMainWindow },
	{ "MessageBox", lm_DeferMessageBox },
	{ "Notebook", lm_DeferNotebook },
//	{ "Outline", lm_DeferOutline },
	{ "PanedWindow", lm_DeferPanedWindow },
	{ "PushButton", lm_DeferPushButton },
	{ "RowColumn", lm_DeferRowColumn },
	{ "Scale", lm_DeferScale },
	{ "ScrollBar", lm_DeferScrollBar },
	{ "ScrolledWindow", lm_DeferScrolledWindow },
	{ "SelectionBox", lm_DeferSelectionBox },
	{ "Separator", lm_DeferSeparator },
	{ "SpinBox", lm_DeferSpinBox },
	{ "TabBox", lm_DeferTabBox },
	{ "TabStack", lm_DeferTabStack },
	{ "Text", lm_DeferText },
	{ "TextField", lm_DeferTextField },
	{ "ToggleButton", lm_DeferToggleButton },
//	{ "Tree", lm_DeferTree },

	{ "BulletinBoardDialog", lm_DeferBulletinBoardDialog },
	{ "ErrorDialog", lm_DeferErrorDialog },
	{ "FileSelectionDialog", lm_DeferFileSelectionDialog },
	{ "FormDialog", lm_DeferFormDialog },
	{ "InformationDialog", lm_DeferInformationDialog },
	{ "MenuBar", lm_DeferMenuBar },
	{ "MessageDialog", lm_DeferMessageDialog },
	{ "OptionMenu", lm_DeferOptionMenu },
	{ "PopupMenu", lm_DeferPopupMenu },
	{ "PromptDialog", lm_DeferPromptDialog },
	{ "PulldownMenu", lm_DeferPulldownMenu },
	{ "QuestionDialog", lm_DeferQuestionDialog },
	{ "RadioBox", lm_DeferRadioBox },
	{ "ScrolledList", lm_DeferScrolledList },
	{ "ScrolledText", lm_DeferScrolledText },
	{ "SelectionDialog", lm_DeferSelectionDialog },
	{ "SimpleCheckBox", lm_DeferSimpleCheckBox },
	{ "SimpleMenuBar", lm_DeferSimpleMenuBar },
	{ "SimpleOptionMenu", lm_DeferSimpleOptionMenu },
	{ "SimplePopupMenu", lm_DeferSimplePopupMenu },
	{ "SimplePulldownMenu", lm_DeferSimplePulldownMenu },
	{ "SimpleRadioBox", lm_DeferSimpleRadioBox },
	{ "WarningDialog", lm_DeferWarningDialog },
	{ "WorkArea", lm_DeferWorkArea },
	{ "WorkingDialog", lm_DeferWorkingDialog },
	{ NULL,			NULL }
};