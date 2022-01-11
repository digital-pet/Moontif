#pragma once

#define WIDGET_CONSTRUCTOR(W) \
  Widget _Construct##W (lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName) { \
	_lua_stackguard_entry(L); \
	Widget wdgWidget; \
	wdgWidget = ConstructGenericWidget(L, parentObj, wdgParent, pszWidgetName, XmCreate##W ); \
	_lua_stackguard_exit(L); \
	return wdgWidget; \
}

#define WIDGET_DEFERRED(W) \
static int lm_Defer##W (lua_State *L)\
{ \
	_lua_stackguard_entry(L); \
	lua_pushlightuserdata(L, _Construct##W ); \
	lua_setfield(L, -2, "__widgetConstructor"); \
	_lua_stackguard_exit(L); \
	return 1; \
}

#define GENERIC_WIDGET_CONSTRUCTOR(W) \
  Widget _Construct##W (lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName) { \
	_lua_stackguard_entry(L); \
	Widget wdgWidget; \
	wdgWidget = ConstructGenericWidget(L, parentObj, wdgParent, pszWidgetName, XmCreate##W ); \
	_lua_stackguard_exit(L); \
	return wdgWidget; \
}

#define COMPLEX_WIDGET_CONSTRUCTOR(W) \
  Widget _Construct##W (lua_State* L, int parentObj, Widget wdgParent, const char* pszWidgetName) { \
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
