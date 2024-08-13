/*
 * jsonmg - JSON parsing library for lua
 *
 *   Copyright (C) 2024 TheRootED24
 *
 *   This program is free software: you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the Free Software Foundation, 
 *   either version 3 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without 
 *   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 *   Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program.
 *  If not, see <https://www.gnu.org/licenses/>
 */

#ifndef JSONMG_H
#define JSONMG_H

#define JSON_MG "jsonmg"
// C++ GAURDS FOR C LIBS
#ifndef __cplusplus
// LUA LIBS FOR gcc
#include <lua.h>                               
#include <lauxlib.h>                           
#include <lualib.h>
#endif

#ifdef __cplusplus
// LUA LIBS FOR g++
#include <lua.hpp>
extern "C" {
#endif
// MONGOOSE C LIB
#include <mongoose.h>
//#include <mgjson.h>

#ifdef __cplusplus
}
#endif

#define MAX_DEPTH 100
#define MAX_JSON_LEN 4096
#define DEBUG 1

#define btoa(x) ((x)?"true":"false")

bool useAjson = false, useSjson = false, isArray = false;

enum JSON_TYPES
{
        JSON_NUMBER_TYPE,       // 0
        JSON_BOOL_TYPE,         // 1
        JSON_LONG_TYPE,         // 2
        JSON_STRING_TYPE,       // 3
        JSON_ARRAY_TYPE,        // 4
        JSON_OBJECT_TYPE,       // 5
        JSON_NESTED_ARRAY_TYPE,
        JSON_NESTED_OBJECT_TYPE,
        JSON_ROOT_TYPE          // 6
};

typedef struct
{
        int depth;
        int ttype;
        int rtype;
        int ptype;
        int pindex;
        int ctype;
        int cindex;
        int closeType;
        int nestArrs;
        int nestObjs;

        bool nestedP;
        bool nestedC;

        int ktype;
        int vtype;
        int nkey;

        bool isRoot;
        bool isParent;
        bool isChild;
        bool isNested;

        // MODE FLAGS
        bool unSuppoted;
        bool parentMode;
        bool childMode;

        const char *close_type;
        const char *key;
        char *json;

        int (*get_type)();

} parseLua;

static char buf[MAX_JSON_LEN];
static int depth = 0;

static double num = 0;
static bool b;
static long l;
static char *sstr;

bool isArrStr = false, isArrNum = false, isArrObj = false;

// COMMA CONTROL
static const char *nextJson = ",";

// KEY/VALUE TYPES
static const char *kvString = "\"%s\":\"%s\"";
static const char *kvBool = "\"%s\":%s";
static const char *kvNumber = "\"%s\":%f";
static const char *nestedkvString = "{\"%s\":\"%s\"}";
static const char *nestedkvNumber = "{\"%s\":%f}";
static const char *nestedkvBool = "{\"%s\":%s}";

// ARRAY VAULE TYPES
static const char *arrayString = "\"%s\"";
static const char *arrayNumber = "%f";
static const char *arrayBool = "%s";

// OBJECT TYPES
static const char *newObject = "{";
static const char *endObject = "}";     
static const char *newKeyObject = "\"%s\":{";
static const char *newNestedKeyObject = "{\"%s\":{";
static const char *endNestedKeyObject = "}}";

// ARRAY TYPES
static const char *newArray = "[";
static const char *endArray = "]";
static const char *newKeyArray = "\"%s\":[";
static const char *nestedKeyArray = "{\"%s\":[";
static const char *endNestedKeyArray = "]}";

#endif /* JSONMG_H */
