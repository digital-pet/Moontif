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

#include <Xm/ButtonBox.h>
#include <Xm/FontS.h>
#include <Xm/Tree.h>
#include <Xm/Outline.h>
#include <Xm/IconBox.h>

#include "include/common.h"
#include "include/utils.h"
#include "include/widgets.h"
#include "include/widgetfactory.h"


#include "WidgetClasses/GenericWidget.h"
#include "WidgetClasses/SimpleMenuBar.h"

void NewWidget(lua_State* L, WidgetConstructor Constructor)
{

	SetGlobalID(L);
	lua_pushlightuserdata(L, Constructor);
	lua_setfield(L, -2, "__widgetConstructor");
	return;
}

void SetGlobalID(lua_State* L) {
	static long long iter = 0;

	// look for global __widgetOrder table
	lua_getglobal(L, "__widgetOrder");

	// if none, create it
	if (!(lua_type(L, -1) == LUA_TTABLE)) {
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "__widgetOrder");
	}
	// Copy the table to the top to use as the key, and make iter the value
	lua_pushvalue(L, 1);
	lua_pushinteger(L, iter);
	lua_rawset(L, -3);

	// throw away all the stuff we put on the stack
	lua_settop(L, 1);
	
	lua_pushinteger(L, iter);
	lua_setfield(L, -2, "__id");
	printf("Widget %llu \n", iter);  // better debugging through printf
	iter++;

	return;
}

//	This file (ab)uses preprocessor macros (see widgets.h) to create a pseudoclass of sorts.
// 
//	GENERIC_WIDGET(<WidgetType>) expands out into two functions:
//		* int lm_Defer<WidgetType>(lua_State *L)
//			- creates a private field in the table it's passed which is 
//				a function pointer to the constructor callback which will
//				be invoked when motif::Realize is called.
// 
//		* Widget _Construct<WidgetType>(lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName)
//			- The corresponding callback method, which itself will call ConstructGenericWidget
//				with a pointer to the actual widget's constructor - XmCreate<WidgetType>
//
//	COMPLEX_WIDGET(<WidgetType>) also expands out into two functions:
//		* int lm_Defer<WidgetType>(lua_State *L)
//			- Identical to its counterpart above
//
//		* Widget _Construct<WidgetType>(lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName)
//			- This will call out to a function Construct<WidgetType> which must take
//				the exact same parameters as are passed to _Construct<WidgetType>
//

/*
 *
 *  CORE GADGETS
 *
 */

GENERIC_WIDGET(ArrowButtonGadget)

GENERIC_WIDGET(LabelGadget)

GENERIC_WIDGET(PushButtonGadget)

GENERIC_WIDGET(SeparatorGadget)

GENERIC_WIDGET(ToggleButtonGadget)

GENERIC_WIDGET(ArrowButton)

/*
 *
 *  CORE WIDGETS
 *
 */

GENERIC_WIDGET(BulletinBoard)
 
GENERIC_WIDGET(ButtonBox)
 
GENERIC_WIDGET(CascadeButton)
 
GENERIC_WIDGET(Column)
 
GENERIC_WIDGET(ComboBox)
 
GENERIC_WIDGET(Command)
 
GENERIC_WIDGET(Container)
 
GENERIC_WIDGET(DataField)
 
GENERIC_WIDGET(DialogShell)
 
GENERIC_WIDGET(DrawingArea)
 
GENERIC_WIDGET(DrawnButton)
 
GENERIC_WIDGET(FileSelectionBox)
 
GENERIC_WIDGET(FontSelector)
 
GENERIC_WIDGET(Form)
 
GENERIC_WIDGET(Frame)
 
GENERIC_WIDGET(IconBox)
 
GENERIC_WIDGET(Label)
 
GENERIC_WIDGET(List)
 
GENERIC_WIDGET(MainWindow)
 
GENERIC_WIDGET(MessageBox)
 
GENERIC_WIDGET(Notebook)
 
GENERIC_WIDGET(Outline)
 
GENERIC_WIDGET(PanedWindow)
 
GENERIC_WIDGET(PushButton)
 
GENERIC_WIDGET(RowColumn)
 
GENERIC_WIDGET(Scale)
 
GENERIC_WIDGET(ScrollBar)
 
GENERIC_WIDGET(ScrolledWindow)
 
GENERIC_WIDGET(SelectionBox)
 
GENERIC_WIDGET(Separator)
 
GENERIC_WIDGET(SpinBox)
 
GENERIC_WIDGET(TabBox)
 
GENERIC_WIDGET(TabStack)
 
GENERIC_WIDGET(Text)
 
GENERIC_WIDGET(TextField)
 
GENERIC_WIDGET(ToggleButton)
 
GENERIC_WIDGET(Tree)
 
/*
 *
 *  META WIDGETS
 *
 */

GENERIC_WIDGET(BulletinBoardDialog)
 
GENERIC_WIDGET(ErrorDialog)
 
GENERIC_WIDGET(FileSelectionDialog)
 
GENERIC_WIDGET(FormDialog)
 
GENERIC_WIDGET(InformationDialog)
 
GENERIC_WIDGET(MenuBar)
 
GENERIC_WIDGET(MessageDialog)
 
GENERIC_WIDGET(OptionMenu)
 
GENERIC_WIDGET(PopupMenu)
 
GENERIC_WIDGET(PromptDialog)
 
GENERIC_WIDGET(PulldownMenu)
 
GENERIC_WIDGET(QuestionDialog)
 
GENERIC_WIDGET(RadioBox)
 
GENERIC_WIDGET(ScrolledList)
 
GENERIC_WIDGET(ScrolledText)
 
GENERIC_WIDGET(SelectionDialog)
 
GENERIC_WIDGET(SimpleCheckBox)

COMPLEX_WIDGET(SimpleMenuBar)

GENERIC_WIDGET(SimpleOptionMenu)
 
GENERIC_WIDGET(SimplePopupMenu)
 
GENERIC_WIDGET(SimplePulldownMenu)
 
GENERIC_WIDGET(SimpleRadioBox)
 
GENERIC_WIDGET(WarningDialog)
 
GENERIC_WIDGET(WorkArea)
 
GENERIC_WIDGET(WorkingDialog)
 
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