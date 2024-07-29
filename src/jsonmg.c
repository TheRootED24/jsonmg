#include "jsonmg.h"

static void stringify(lua_State *L, int index, parseLua *self)
{
        parseLua pL = {.ttype = -1, .ptype = -1, .ctype = -1, .ktype = -1, .vtype = -1, .nkey = 0, .isRoot = false, 
                                                .isParent = false, .isChild = false, .isNested = false, .json = buf, .key = NULL};

        int tableType = lua_objlen(L, index) > 1 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
               
        if(self->isRoot)
        {
                if(tableType == JSON_ARRAY_TYPE)
                {
                        pL.ttype = JSON_ARRAY_TYPE;
                        tableType = JSON_ARRAY_TYPE;
                        lua_pushstring(L, self->json);
                        lua_pushstring(L, newArray);
                        lua_concat(L, 2);
                        strcpy(self->json, lua_tostring(L, -1));
                        lua_remove(L, -1); 
                }
                else if(tableType == JSON_OBJECT_TYPE)
                {
                        pL.ttype = JSON_OBJECT_TYPE;
                        tableType = JSON_OBJECT_TYPE;
                        lua_pushstring(L, self->json);
                        lua_pushstring(L, newObject);
                        lua_concat(L, 2);
                        strcpy(self->json, lua_tostring(L, -1));
                        lua_remove(L, -1);
                }

                pL.isRoot = true;
                self->isParent = true;
                self->isRoot = false;
        }

        pL.ttype = tableType;
        self->isParent = true;

        lua_pushvalue(L, index);
        lua_pushnil(L);

        while (lua_next(L, -2))
        {
                if(self->isParent != 1 && self->isChild == 0)
                {
                        // comma control
                        lua_pushstring(L, self->json);
                        lua_pushstring(L, nextJson);
                        lua_concat(L, 2);
                        strcpy(self->json, lua_tostring(L, -1));
                        lua_remove(L, -1);

                }
                // copy the key so that lua_tostring does not modify the original
                lua_pushvalue(L, -2);
                // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
                int keytype = lua_type(L, -1);
                int valtype = lua_type(L, -2);

                pL.ktype = keytype;
                pL.vtype = valtype;

                self->ttype = tableType;
                self->ktype = keytype;
                self->vtype = valtype;
                /* KEY */
                switch (keytype)
                {
                        /* KEY = TABLE*/
                        case LUA_TTABLE:
                        {       /* VAL */
                                switch(valtype)
                                {       /* KEY = TABLE : VAL = STRING*/
                                        case LUA_TSTRING:
                                        {
                                                tableType = lua_objlen(L, index) > 0 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                                                const char *key = lua_tostring(L, -2);
                                                if(tableType == JSON_ARRAY_TYPE )
                                                {
                                                        //printf("%s:[", key);
                                                        lua_pushstring(L, self->json);
                                                        lua_pushfstring(L, newKeyArray, key);
                                                        lua_concat(L, 2);
                                                        strcpy(self->json, lua_tostring(L, -1));
                                                        lua_remove(L, -1);
                                                }         
                                                else
                                                {
                                                        //printf("%s:{", key);
                                                        lua_pushstring(L, self->json);
                                                        lua_pushfstring(L, newKeyObject, key);
                                                        lua_concat(L, 2);
                                                        strcpy(self->json, lua_tostring(L, -1));
                                                        lua_remove(L, -1); 
                                                }
                                                break; 
                                        }
                                        /* KEY = TABLE : VAL = NUMBER */
                                        case LUA_TNUMBER:
                                        {
                                                tableType = lua_objlen(L, index) > 0 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                                                if(tableType == JSON_ARRAY_TYPE && lua_objlen(L, -1) > 0 )
                                                {
                                                        //printf("[");
                                                        lua_pushstring(L, self->json);
                                                        lua_pushstring(L, newArray);
                                                        lua_concat(L, 2);
                                                        strcpy(self->json, lua_tostring(L, -1));
                                                        lua_remove(L, -1);
                                                }              
                                                else
                                                {
                                                        //printf("{");
                                                        lua_pushstring(L, self->json);
                                                        lua_pushstring(L, newObject);
                                                        lua_concat(L, 2);
                                                        strcpy(self->json, lua_tostring(L, -1));
                                                        lua_remove(L, -1);
                                                        self->isNested = true;
                                                } 
                                                break; 
                                        }

                                }
                                self->isParent = true;
                                stringify(L, -2, self);
                                tableType = lua_objlen(L, index) > 0 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                                self->isParent = true;
                                break;
                        }
                        /* KEY IS A STRING || NUMBER*/
                        case LUA_TSTRING:
                        case LUA_TNUMBER:
                        {       /* VAL -2*/
                                switch (valtype)
                                {
                                        /* VAL IS A TABLE -2 */
                                        case LUA_TTABLE:
                                        {       /* comma control, parents don't get commas*/
                                                self->isParent = true;
                                                /* KEY = NUM -1: VAL = TABLE -2*/
                                                if(keytype ==  LUA_TNUMBER)
                                                {
                                                        tableType = lua_objlen(L, index) > 0 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                                                        if(tableType == JSON_ARRAY_TYPE && lua_objlen(L, -2) > 0 )
                                                        {
                                                                //printf("[");
                                                                lua_pushstring(L, self->json);
                                                                lua_pushstring(L, newArray);
                                                                lua_concat(L, 2);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1);
                                                        }
                                                                
                                                        else if(tableType == JSON_OBJECT_TYPE || (tableType == JSON_ARRAY_TYPE && lua_objlen(L, -2) == 0 && self->isNested == 0) )
                                                        {
                                                                //printf("{");
                                                                lua_pushstring(L, self->json);
                                                                lua_pushstring(L, "{");
                                                                lua_concat(L, 2);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1);
                                                                self->isNested = true;
                                                        }
                                                        
                                                        
                                                }
                                                 /* KEY = STRING -1: VAL = TABLE -2*/    
                                                else if(keytype == LUA_TSTRING)
                                                {
                                                        tableType = lua_objlen(L, index) > 1 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                                                        const char *key = lua_tostring(L, -1);
                                                        pL.key = key;
                                                        if(tableType == JSON_ARRAY_TYPE && lua_objlen(L, -2) > 0)
                                                        {
                                                                //printf("%s:[", key);
                                                                lua_pushstring(L, self->json);
                                                                lua_pushfstring(L, newKeyArray, key);
                                                                lua_concat(L, 2);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1);
                                                        }    
                                                        else if(tableType == JSON_OBJECT_TYPE)
                                                        {
                                                                if(lua_objlen(L, -2) > 0)
                                                                {
                                                                        //printf("%s:[", key);
                                                                        lua_pushstring(L, self->json);
                                                                        lua_pushfstring(L, newKeyArray, key);
                                                                        lua_concat(L, 2);
                                                                        strcpy(self->json, lua_tostring(L, -1));
                                                                        lua_remove(L, -1);
                                                                }
                                                                else
                                                                {
                                                                        //printf("%s:{", key);
                                                                        lua_pushstring(L, self->json);
                                                                        lua_pushfstring(L, newKeyObject, key);
                                                                        lua_concat(L, 2);
                                                                        strcpy(self->json, lua_tostring(L, -1));
                                                                        lua_remove(L, -1); 
                                                                }
                                                        }
                                                        else if(self->isNested == 0)
                                                        {
                                                                //printf("%s:{", key);
                                                                lua_pushstring(L, self->json);
                                                                lua_pushfstring(L, nestedKeyObject, key);
                                                                lua_concat(L, 2);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1); 
                                                        }
                                                        else if(self->isNested == 1)
                                                        {
                                                                //printf("%s:{", key);
                                                                lua_pushstring(L, self->json);
                                                                lua_pushfstring(L, newKeyObject, key);
                                                                lua_concat(L, 2);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1); 
                                                        }
                                                }
                                                        
                                                // RECURSION
                                                stringify(L, -2, self);
                                                tableType = lua_objlen(L, index) > 0 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                                                self->isParent = false;
                                                self->isNested = false;
                                                break;
                                        }
                                        /* VAL */
                                        case LUA_TSTRING:
                                        case LUA_TNUMBER:
                                        {
                                                self->isParent = false;
                                                 /* KEY = STRING -1: VAL = STRING -2*/
                                                if(valtype == LUA_TSTRING)
                                                {
                                                        const char *key = lua_tostring(L, -1);
                                                        const char *val = lua_tostring(L, -2);
                                                        tableType = lua_objlen(L, index) > 0 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                                                        /* KEY = NUMBER -1 */
                                                        if(keytype == LUA_TNUMBER)
                                                        {
                                                                //printf("\"%s\"",  val);
                                                                lua_pushstring(L, self->json);
                                                                lua_pushfstring(L, arrayString, val);
                                                                lua_concat(L, 2);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1);
                                                        }
                                                        /* KEY = STRING -1: VAL = STRING -2*/
                                                        else if(keytype == LUA_TSTRING && tableType == JSON_OBJECT_TYPE && self->isNested == 1)
                                                        {
                                                                //printf("%s:\"%s\"", key,val);
                                                                lua_pushstring(L, self->json);
                                                                lua_pushfstring(L, kvString, key, val);
                                                                lua_concat(L, 2);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1);
                                                        }
                                                         else if(keytype == LUA_TSTRING && (tableType == JSON_ARRAY_TYPE && self->isNested == 0 ))
                                                        {
                                                                //printf("%s:\"%s\"", key,val);
                                                                lua_pushstring(L, self->json);
                                                                lua_pushstring(L, newObject);
                                                                lua_pushfstring(L, kvString, key, val);
                                                                lua_pushstring(L, endObject);
                                                                lua_concat(L, 4);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1);
                                                        }
                                                         else if(keytype == LUA_TSTRING && (tableType == JSON_ARRAY_TYPE && self->isNested == 1 ))
                                                        {
                                                                //printf("%s:\"%s\"", key,val);
                                                                lua_pushstring(L, self->json);
                                                                lua_pushfstring(L, kvString, key, val);
                                                                lua_concat(L, 2);
                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                lua_remove(L, -1);
                                                        }
                                                }  
                                                /* VAL = NUMBER -2*/
                                                else if(valtype == LUA_TNUMBER)
                                                {
                                                        switch(tableType)
                                                        {
                                                                case JSON_OBJECT_TYPE:
                                                                {
                                                                        /* KEY = STRING -1: VAL = NUMBER -2*/
                                                                        if(keytype == LUA_TSTRING)
                                                                        {
                                                                                const char *key = lua_tostring(L, -1);
                                                                                lua_Number lnum = lua_tonumber(L, -2);
                                                                                //printf("%s:%g", key,lnum);
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, kvNumber, key, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                        }
                                                                        /* KEY = NUMBER -1: VAL = NUMBER -2*/
                                                                        else if(keytype == LUA_TNUMBER)
                                                                        {
                                                                                lua_Number lnum = lua_tonumber(L, -2);
                                                                                //printf("%g:%g", lnum,lnum); 
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, idxNumber, lnum, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                
                                                                        }
                                                                        break; 
                                                                }
                                                                case JSON_ARRAY_TYPE:
                                                                {       /* KEY = NUMBER -1: VAL = NUMBER -2*/
                                                                        if(keytype == LUA_TSTRING)
                                                                        {
                                                                                const char *key = lua_tostring(L, -1);
                                                                                lua_Number lnum = lua_tonumber(L, -2);
                                                                                //printf("%s:%g", key,lnum);
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, kvNumber, key, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                
                                                                        }
                                                                        else if(keytype == LUA_TNUMBER)
                                                                        {
                                                                                lua_Number lnum = lua_tonumber(L, -2);
                                                                                //printf("%g", lnum); 
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, arrayNumber, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                
                                                                        }
                                                                        break; 
                                                                }
                                                                
                                                        }

                                                }       
                                                pL.isParent = false;
                                                pL.isChild = true;
                                        }
                                
                                break;
                                }
                        }
                       
                }
                // pop value + copy of key, leaving original key
                tableType = lua_objlen(L, index) > 0 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                lua_pop(L, 2);
                // stack now contains: -1 => key; -2 => table
        }
        switch(pL.ttype)
        {
                case JSON_ARRAY_TYPE:
                {
                        // printf("]");
                        lua_pushstring(L, self->json);
                        lua_pushfstring(L, "%s", endArray);
                        lua_concat(L, 2);
                        strcpy(self->json, lua_tostring(L, -1));
                        lua_remove(L, -1);
                        break;
                }
                case JSON_OBJECT_TYPE:
                {

                        // printf("}");
                        lua_pushstring(L, self->json);
                        lua_pushfstring(L, "%s", endObject);
                        lua_concat(L, 2);
                        strcpy(self->json, lua_tostring(L, -1));
                        lua_remove(L, -1);
                        break;
                }
                break;
        }
        
        // Pop table
        lua_pop(L, 1);
        // Stack is now the same as it was on entry to this function
}

// JSON TYPE CHECKER
// Accepted types are: objects, arrays, strings, numbers, longs, boolens ... all other types are ignored
static int json_type(lua_State *L, const char *key, char *index, struct mg_str *json)
{
        if (*index == '{')
        {
                lua_pushnumber(L, JSON_OBJECT_TYPE);
                return 1;
        }
        if (*index == '[')
        {
                if (*(index + 1) == '{')
                {
                        isArrObj = true;
                        isArrStr = false;
                        isArrNum = false;
                }

                else if ((*(index + 1) == '"') || (*(index + 1) == '\'') || (*(index + 2) == '"') || (*(index + 2) == '\''))
                {
                        isArrStr = true;
                        isArrNum = false;
                }
                else
                {
                        isArrNum = true;
                        isArrStr = false;
                }
                
                lua_pushnumber(L, JSON_ARRAY_TYPE);
                return 1;
        }
        // TODO: the parsed values (if true) are set in global variables and fetch by the calling functionm
        // this is a lazy approach but the simplicity of it,
        // versus the complexity of messing with the busy stack is not worth it ...at this time
        bool isNum = mg_json_get_num(*json, key, &num);
        bool isBool = mg_json_get_bool(*json, key, &b);

        l = mg_json_get_long(*json, key, -1);
        bool isLong = l == -1 ? false : true;

        sstr = mg_json_get_str(*json, key);
        bool isStr = (sstr == NULL) ? false : true;

        if (isNum)
        {
                lua_pushnumber(L, JSON_NUMBER_TYPE);
                return 1;
        }
        else if (isBool)
        {
                lua_pushnumber(L, JSON_BOOL_TYPE);
                return 1;
        }
        else if (isLong)
        {
                lua_pushnumber(L, JSON_LONG_TYPE);
                return 1;
        }
        else if (isStr)
        {
                lua_pushnumber(L, JSON_STRING_TYPE);
                return 1;
        }
        else
                fprintf(stderr, "jsonParse error: [ Invalid parent_type ]\n");
        return 0;
}

static int mg_key(lua_State *L)
{
        const char *str = lua_tostring(L, -1);
        lua_remove(L, -1);
        lua_pushfstring(L, "$.%s", str);
        return 1;
}

bool isRoot = true;
static int json_scan(lua_State *L, struct mg_str json, int depth)
{
        if(isRoot || depth == 0)
                isRoot = false;

        static int idx = 1;
        int n = 0, o = mg_json_get(json, "$", &n);
        if (json.ptr[o] == '{' || json.ptr[o] == '[')
        {
                struct mg_str key, val, sub = mg_str_n(json.ptr + o, (size_t)n);
                size_t ofs = 0;
                while ((ofs = mg_json_next(sub, ofs, &key, &val)) > 0)
                {
                        const char *jkey;
                        bool isKeyed = key.len > 0 ? true : false;

                        if (isKeyed)
                        {       // key.ptr+1 & key.len-2 to remove quotes
                                lua_pushlstring(L, key.ptr + 1, key.len - 2); 
                                // mg_key creates jkey and pushes it onto stack,
                                mg_key(L);
                                // save a copy of jkey
                                jkey = lua_tostring(L, -1);
                                // remove jkey from stack,
                                lua_remove(L, -1);
                                json_type(L, jkey, (char *)val.ptr, &json);
                        }
                        // since we are using the builtin mongoose json_get functions to determine type, we must create a json object,
                        // containing a key:value pair so we can check the VALUE type. 
                        else
                        {
                                // construct a json object with a dummy key and the value to type check value
                                char *buf = mg_mprintf("{ %m: %.*s }", MG_ESC("dkey"), (int)val.len, val.ptr);
                                struct mg_str djson = mg_str(buf);
                                const char *dkey = "$.dkey";

                                json_type(L, dkey, (char *)val.ptr, &djson);
                                free(buf);
                        }

                        int type = lua_tointeger(L, -1); lua_remove(L, -1);
                        switch (type)
                        {
                                case JSON_NUMBER_TYPE:
                                {
                                        if (isKeyed)
                                        {
                                                lua_pushlstring(L, key.ptr+1, key.len-2);
                                                lua_pushnumber(L, num);
                                        }
                                        else
                                        {
                                                lua_pushnumber(L, idx++);
                                                lua_pushnumber(L, num);
                                        }
                                        lua_settable(L, -3);
                                        break;
                                }
                                case JSON_BOOL_TYPE:
                                {
                                        if (isKeyed)
                                        {
                                                lua_pushlstring(L, key.ptr+1, key.len-2);
                                                lua_pushboolean(L, b);
                                        }
                                        else
                                        {
                                                lua_pushnumber(L, idx++);
                                                lua_pushboolean(L, b);
                                        }
                                        lua_settable(L, -3);
                                        break;
                                }
                                case JSON_LONG_TYPE:
                                {
                                        if (isKeyed)
                                        {
                                                lua_pushlstring(L, key.ptr+1, key.len-2);
                                                lua_pushnumber(L, l);
                                        }
                                        else
                                        {
                                                lua_pushnumber(L, idx++);
                                                lua_pushnumber(L, l);
                                        }
                                        lua_settable(L, -3);
                                        break;
                                }
                                case JSON_STRING_TYPE:
                                {
                                        if (isKeyed)
                                        {
                                                lua_pushlstring(L, key.ptr+1, key.len-2);
                                                lua_pushstring(L, sstr);
                                        }
                                        else
                                        {
                                                lua_pushnumber(L, idx++);
                                                lua_pushstring(L, sstr);
                                        }
                                        free(sstr);
                                        lua_settable(L, -3);
                                        break;
                                }
                                case JSON_ARRAY_TYPE:
                                {
                                        if (isKeyed)
                                        {
                                                lua_pushlstring(L, key.ptr+1, key.len-2);
                                                lua_newtable(L);
                                        }
                                        else
                                        {
                                                lua_pushnumber(L, idx++);
                                                lua_newtable(L);   
                                        }
                                        break;
                                }
                                case JSON_OBJECT_TYPE:
                                {
                                        if (isKeyed)
                                        {
                                                lua_pushlstring(L, key.ptr+1, key.len-2);
                                                lua_newtable(L);
                                        }
                                        else
                                        {
                                                lua_pushnumber(L, idx++);
                                                lua_newtable(L);   
                                        }
                                        break;
                                }
                        }
                        // found nested object or array
                        if (*val.ptr == '[' || *val.ptr == '{')
                                json_scan(L, val, depth += 2);

                        // found the end of the current object / array
                        char last = json.ptr[ofs];
                        if (last == ']' || last == '}')
                        {
                                // we're finished parsing when stack size is 1
                                bool finished = lua_gettop(L) == 1 ? true : false;

                                if (!finished)
                                {
                                        lua_settable(L, -3);
                                        depth -= 2;
                                        if (last == ']') idx = 1;
                                }
                        }
                }
        }
        // leave the table on the stack and return it
        if (depth == 1)
                return 1;
  // recursion
  return 0;
}

static int stringify_lua(lua_State *L)
{
        parseLua pL = {.ttype = -1, .ptype = -1, .ctype = -1, .ktype = -1, .vtype = -1, .nkey = 0, .isRoot = true, 
                                        .isParent = false, .isChild = false, .isNested = false, .json = buf, .key = NULL};
        pL.isRoot = true;
        pL.json = buf;
        stringify(L, -1, &pL);
        lua_settop(L,0);
        lua_pushstring(L, pL.json);
        memset(buf, 0, sizeof(buf));
        return 1;
}

static int parse_json(lua_State *L)
{
        struct mg_str json = mg_str(lua_tostring(L, -1));
        static int depth = 0;
        lua_settop(L, 0);
        lua_newtable(L);
        json_scan(L, json, depth);
        return 1;
}

// GAURDS FOR g++ to prevent name mangling during linking
#ifdef __cplusplus
extern "C" {
#endif

static const luaL_reg jsonmg_methods[] = {
  { "parse",	        parse_json      },
  { "stringify",        stringify_lua   },
	{ }
};

int luaopen_jsonmg(lua_State *L)
{
	luaL_register(L, JSON_MG, jsonmg_methods);
	return 1;
}

#ifdef __cplusplus
}
#endif
