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

#include "include/common.h"
#include "include/utils.h"

char* gc_strdup(const char* s) {
	int size = strlen(s) + 1;
	char* p = GC_MALLOC(size);
	if (p) {
		memset(p, 0, size);
		memcpy(p, s, size - 1);
	}
	return p;
}

// notation:
// s = source
// k = key
// t = table
// x = unacceptable value

void GetRegistry(lua_State* L) {
    lua_pushlightuserdata(L, (void*)&registryKey);      // k
    lua_gettable(L, LUA_REGISTRYINDEX);                 // t or x
    if (!(lua_type(L, -1) == LUA_TTABLE)) {             // if x
        lua_pop(L, 1);                                  // _
        lua_newtable(L);                                // t
        lua_pushlightuserdata(L, (void*)&registryKey);  // t k
        lua_pushvalue(L, -2);                           // t k t
        lua_settable(L, LUA_REGISTRYINDEX);             // t
    }
}

void GetOrCreateTable(lua_State* L, int idx) {
    int absolute = lua_absindex(L, idx);        // assuming idx == -2 && absolute == 1
    lua_pushvalue(L, -1);                       // s k -> s k k
    lua_gettable(L, absolute);                  // s k k -> s k t || s k x
    if (!(lua_type(L, -1) == LUA_TTABLE)) {     // if s k x
        lua_pop(L, 1);                          // s k
        lua_newtable(L);                        // s k t
        lua_pushvalue(L, -1);                   // s k t t
        lua_rotate(L, -3, 1);                   // s k t t -> s t k t
        lua_settable(L, absolute);              // s t
    } 
    else {
        lua_remove(L, -2);                      // s k t -> s t
    }
}

void dumpstack(lua_State* L) {
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(L, i));
        switch (lua_type(L, i)) {
        case LUA_TNUMBER:
            printf("%g\n", lua_tonumber(L, i));
            break;
        case LUA_TSTRING:
            printf("%s\n", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            printf("%s\n", "nil");
            break;
        default:
            printf("%p\n", lua_topointer(L, i));
            break;
        }
    }
}