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

#ifdef __cplusplus
}
#endif

#define MAX_DEPTH 100
#define MAX_JSON_LEN 4096

bool useAjson = false, useSjson = false, isArray = false;

enum
{
        JSON_NUMBER_TYPE, // 0
        JSON_BOOL_TYPE,   // 1
        JSON_LONG_TYPE,   // 2
        JSON_STRING_TYPE, // 3
        JSON_ARRAY_TYPE,  // 4
        JSON_OBJECT_TYPE, // 5
        JSON_ROOT_TYPE    // 6
};

typedef struct
{
        int ttype;
        int ptype;
        int ctype;

        int ktype;
        int vtype;
        int nkey;

        bool isRoot;
        bool isParent;
        bool isChild;
        bool isNested;
        char *json;
        const char *key;
} parseLua;



static double num = 0;
static bool b;
static long l;
static char *sstr;


int nextClose[MAX_DEPTH];

bool isArrStr = false, isArrNum = false, isArrObj = false;

static char buf[MAX_JSON_LEN];
static const char *nextJson = ",";
static const char *kvString = "%s:\"%s\"";
static const char *kvNumber = "%s:%f";
static const char *idxNumber = "%f:%f";
static const char *newKeyArray = "%s:[";
static const char *newArray = "[";
static const char *arrayString = "\"%s\"";
static const char *arrayNumber = "%f";
static const char *endArray = "]";
static const char *newObject = "{";
static const char *newKeyObject = "%s:{";
static const char *nestedKeyObject = "{%s:";
static const char *endObject = "}";

#endif /* JSONMG_H */
