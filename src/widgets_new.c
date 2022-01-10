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

#include "include/luamotif.h"
#include "include/widgetfactory.h"

#define CreateWidget(W) \
Widget Construct##W##(Widget wdgParent,String szClassName, ArgList args, Cardinal nArgs) { \
	Widget wdgWidget; \
	wdgWidget = XmCreate##W##(wdgParent, szClassName, args, nArgs); \
	return wdgWidget; \
}

#define Defer(W) \
static int lm_Defer##W##(lua_State *L)\
{ \
	lua_pushlightuserdata(L, Construct##W##); \
	lua_setfield(L, -2, "__widgetConstructor"); \
	lua_pushstring(L, #W); \
	lua_setfield(L, -2, "__widgetKind");\
	return 1; \
}

/*
 *
 *  CORE GADGETS
 *
 */

CreateWidget(ArrowButtonGadget)
Defer(ArrowButtonGadget)

CreateWidget(LabelGadget)
Defer(LabelGadget)

CreateWidget(PushButtonGadget)
Defer(PushButtonGadget)

CreateWidget(SeparatorGadget)
Defer(SeparatorGadget)

CreateWidget(ToggleButtonGadget)
Defer(ToggleButtonGadget)

CreateWidget(ArrowButton)
Defer(ArrowButton)

/*
 *
 *  CORE WIDGETS
 *
 */

CreateWidget(BulletinBoard)
Defer(BulletinBoard)

CreateWidget(ButtonBox)
Defer(ButtonBox)

CreateWidget(CascadeButton)
Defer(CascadeButton)

CreateWidget(Column)
Defer(Column)

CreateWidget(ComboBox)
Defer(ComboBox)

CreateWidget(Command)
Defer(Command)

CreateWidget(Container)
Defer(Container)

CreateWidget(DataField)
Defer(DataField)

CreateWidget(DialogShell)
Defer(DialogShell)

CreateWidget(DrawingArea)
Defer(DrawingArea)

CreateWidget(DrawnButton)
Defer(DrawnButton)

CreateWidget(FileSelectionBox)
Defer(FileSelectionBox)

CreateWidget(FontSelector)
Defer(FontSelector)

CreateWidget(Form)
Defer(Form)

CreateWidget(Frame)
Defer(Frame)

CreateWidget(Hierarchy)
Defer(Hierarchy)

CreateWidget(IconBox)
Defer(IconBox)

CreateWidget(Label)
Defer(Label)

CreateWidget(List)
Defer(List)

CreateWidget(MainWindow)
Defer(MainWindow)

CreateWidget(MessageBox)
Defer(MessageBox)

CreateWidget(Notebook)
Defer(Notebook)

CreateWidget(Outline)
Defer(Outline)

CreateWidget(PanedWindow)
Defer(PanedWindow)

CreateWidget(PushButton)
Defer(PushButton)

CreateWidget(RowColumn)
Defer(RowColumn)

CreateWidget(Scale)
Defer(Scale)

CreateWidget(ScrollBar)
Defer(ScrollBar)

CreateWidget(ScrolledWindow)
Defer(ScrolledWindow)

CreateWidget(SelectionBox)
Defer(SelectionBox)

CreateWidget(Separator)
Defer(Separator)

CreateWidget(SpinBox)
Defer(SpinBox)

CreateWidget(TabBox)
Defer(TabBox)

CreateWidget(TabStack)
Defer(TabStack)

CreateWidget(Text)
Defer(Text)

CreateWidget(TextField)
Defer(TextField)

CreateWidget(ToggleButton)
Defer(ToggleButton)

CreateWidget(Tree)
Defer(Tree)

/*
 *
 *  META WIDGETS
 *
 */

CreateWidget(BulletinBoardDialog)
Defer(BulletinBoardDialog)

CreateWidget(ErrorDialog)
Defer(ErrorDialog)

CreateWidget(FileSelectionDialog)
Defer(FileSelectionDialog)

CreateWidget(FormDialog)
Defer(FormDialog)

CreateWidget(InformationDialog)
Defer(InformationDialog)

CreateWidget(MenuBar)
Defer(MenuBar)

CreateWidget(MessageDialog)
Defer(MessageDialog)

CreateWidget(OptionMenu)
Defer(OptionMenu)

CreateWidget(PopupMenu)
Defer(PopupMenu)

CreateWidget(PromptDialog)
Defer(PromptDialog)

CreateWidget(PulldownMenu)
Defer(PulldownMenu)

CreateWidget(QuestionDialog)
Defer(QuestionDialog)

CreateWidget(RadioBox)
Defer(RadioBox)

CreateWidget(ScrolledList)
Defer(ScrolledList)

CreateWidget(ScrolledText)
Defer(ScrolledText)

CreateWidget(SelectionDialog)
Defer(SelectionDialog)

CreateWidget(SimpleCheckBox)
Defer(SimpleCheckBox)

CreateWidget(SimpleMenuBar)
Defer(SimpleMenuBar)

CreateWidget(SimpleOptionMenu)
Defer(SimpleOptionMenu)

CreateWidget(SimplePopupMenu)
Defer(SimplePopupMenu)

CreateWidget(SimplePulldownMenu)
Defer(SimplePulldownMenu)

CreateWidget(SimpleRadioBox)
Defer(SimpleRadioBox)

CreateWidget(WarningDialog)
Defer(WarningDialog)

CreateWidget(WorkArea)
Defer(WorkArea)

CreateWidget(WorkingDialog)
Defer(WorkingDialog)

struct luaL_Reg lm_widgetConstructors[] = {
	{ "ArrowButtonGadget", lm_DeferArrowButtonGadget },
	{ "LabelGadget", lm_DeferLabelGadget },
	{ "PushButtonGadget", lm_DeferPushButtonGadget },
	{ "SeparatorGadget", lm_DeferSeparatorGadget },
	{ "ToggleButtonGadget", lm_DeferToggleButtonGadget },

	{ "ArrowButton", lm_DeferArrowButton },
	{ "BulletinBoard", lm_DeferBulletinBoard },
	{ "ButtonBox", lm_DeferButtonBox },
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
	{ "FontSelector", lm_DeferFontSelector },
	{ "Form", lm_DeferForm },
	{ "Frame", lm_DeferFrame },
	{ "Hierarchy", lm_DeferHierarchy },
	{ "IconBox", lm_DeferIconBox },
	{ "Label", lm_DeferLabel },
	{ "List", lm_DeferList },
	{ "MainWindow", lm_DeferMainWindow },
	{ "MessageBox", lm_DeferMessageBox },
	{ "Notebook", lm_DeferNotebook },
	{ "Outline", lm_DeferOutline },
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
	{ "Tree", lm_DeferTree },

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