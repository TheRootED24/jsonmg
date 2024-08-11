#include "jsonmg.h"

// PARSER CONTROL MODES
static int parse_mode   = -1, 
           root_type    = -1,
           parent_type  = -1;

// JSON NEXT CLOSE TYPE STACK
const char* next_close[MAX_DEPTH];
static int nc_cnt = 1;

static void push_root_close(const char *closeType)
{
        next_close[0] = closeType;     
}

static const char* get_root_close()
{
        return next_close[0];   
}

static void pop_root_close()
{
        next_close[0] = NULL;
}

static void push_next_close(const char *closeType)
{
        next_close[nc_cnt++] = closeType;
        
}

static const char* get_next_close()
{
        if(nc_cnt > 0)    
                return(next_close[nc_cnt - 1]);
        else
                return NULL;    
}

static void pop_next_close()
{
        if(nc_cnt > 1)
        {
                nc_cnt--;
                next_close[nc_cnt] = NULL;
        }
}

static int get_parent_type()
{
        const char* ptype = get_next_close();
        if(ptype == NULL)
                ptype = get_root_close();
        if((strcmp(ptype, endObject) == 0))
                return JSON_OBJECT_TYPE;
        else if((strcmp(ptype, endArray) == 0))
                return JSON_ARRAY_TYPE;
        else if((strcmp(ptype, endNestedKeyObject) == 0))
                return JSON_NESTED_OBJECT_TYPE;
        else if((strcmp(ptype, endNestedKeyArray) == 0))
                return JSON_NESTED_ARRAY_TYPE;
        else
                return -1; 
}

static void set_json_root(int parse_type)
{
        root_type = parse_type;                        
        parent_type = parse_type;
        parse_mode = parse_type;
        return;
}

static void stepDepth(char sign, int level)
{
        if(level > 1 && depth > 0) depth = 0;
        if(level == 1 && sign == '-' && depth > 0) depth--;
        if(level == 1 && sign == '+') depth++;  
}


static void stringify(lua_State *L, int index, parseLua *self)
{       
        int tableType = lua_objlen(L, index) > 1 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
               
        if(self->isRoot)
        {
                lua_pushstring(L, self->json);
                tableType == JSON_ARRAY_TYPE ? lua_pushstring(L, newArray) : lua_pushstring(L, newObject);
                tableType == JSON_ARRAY_TYPE ? push_root_close(endArray) : push_root_close(endObject);
                root_type = tableType;
                set_json_root(tableType); 
                lua_concat(L, 2);
                strcpy(self->json, lua_tostring(L, -1));
                lua_remove(L, -1);

                self->isParent = false;
                self->isRoot = true;
                self->rtype = tableType;
                self->ptype = parent_type;
                self->ctype = parse_mode;
                depth = 0;
        }

        self->parentMode = true;
        self->isParent = true;
        self->isRoot = false;

        lua_pushvalue(L, index);
        lua_pushnil(L);

        while (lua_next(L, -2))
        {
                if(!self->isParent && self->isChild && !self->unSuppoted)
                {
                        // comma control
                        lua_pushstring(L, self->json);
                        lua_pushstring(L, nextJson);
                        lua_concat(L, 2);
                        strcpy(self->json, lua_tostring(L, -1));
                        lua_remove(L, -1);

                }
                self->unSuppoted = false;
                // copy the key so that lua_tostring does not modify the original
                lua_pushvalue(L, -2);
                // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
                int keytype = lua_type(L, -1);
                int valtype = lua_type(L, -2);

                self->ttype = tableType;
                self->ktype = keytype;
                self->vtype = valtype;
               

                /* KEY IS A STRING || NUMBER */
                switch (valtype)
                {      
                        case LUA_TTABLE:
                        {       
                                parse_mode = lua_objlen(L, -2) > 0 ? JSON_ARRAY_TYPE : JSON_OBJECT_TYPE;
                                
                                // GET THE ROOT/PARENT TYPE TO DETERMINE IF NESTING IS NEEDED
                                if(nc_cnt > 1)
                                        parent_type = get_parent_type();
                                else
                                        parent_type = root_type;

                                self->isParent = true;
                                self->isChild = false;
                                
                                self->ptype = parent_type;
                                self->ctype = parse_mode;
                                self->vtype = LUA_TTABLE;
                                self->childMode = false;

                                switch(keytype)
                                {      
                                        case LUA_TSTRING:
                                        case LUA_TNUMBER:
                                        {
                                                switch(parent_type)
                                                {
                                                        case JSON_ARRAY_TYPE:
                                                        case JSON_NESTED_ARRAY_TYPE:
                                                        {
                                                                switch(parse_mode)
                                                                {
                                                                        
                                                                        case JSON_ARRAY_TYPE:
                                                                        case JSON_NESTED_ARRAY_TYPE:
                                                                        {
                                                                                const char *key = lua_tostring(L, -1);

                                                                                switch(keytype)
                                                                                {
                                                                                        case LUA_TSTRING:
                                                                                        {
                                                                                                lua_pushstring(L, self->json);
                                                                                                lua_pushfstring(L, nestedKeyArray, key);
                                                                                                lua_concat(L, 2);
                                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                                lua_remove(L, -1);
                                                                                                push_next_close(endNestedKeyArray);
                                                                                                stepDepth('-', 2);
                                                                                                break;
                                                                                        }
                                                                                        case LUA_TNUMBER:
                                                                                        {
                                                                                                lua_pushstring(L, self->json);
                                                                                                lua_pushfstring(L, newArray, key);
                                                                                                lua_concat(L, 2);
                                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                                lua_remove(L, -1);
                                                                                                push_next_close(endArray);
                                                                                                stepDepth('-', 2);
                                                                                                break;
                                                                                        }
                                                                                        break;
                                                                                }
                                                                                break;
                                                                        }
                                                                        case JSON_OBJECT_TYPE:
                                                                        case JSON_NESTED_OBJECT_TYPE:
                                                                        {
                                                                                const char *key = lua_tostring(L, -1);
                                                                                
                                                                                switch(keytype)
                                                                                {
                                                                                        case LUA_TSTRING:
                                                                                        {
                                                                                                lua_pushstring(L, self->json);
                                                                                                if(parent_type != JSON_OBJECT_TYPE)
                                                                                                {
                                                                                                        lua_pushfstring(L, newNestedKeyObject, key);
                                                                                                        push_next_close(endNestedKeyObject);
                                                                                                }
                                                                                                        
                                                                                                else
                                                                                                {
                                                                                                        lua_pushfstring(L, newKeyObject, key);
                                                                                                        push_next_close(endObject);
                                                                                                }
                                                                                                lua_concat(L, 2);
                                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                                lua_remove(L, -1);
                                                                                                stepDepth('+', 1);
                                                                                                break;
                                                                                        }
                                                                                        case LUA_TNUMBER:
                                                                                        {
                                                                                                lua_pushstring(L, self->json);
                                                                                                lua_pushfstring(L, newObject);
                                                                                                lua_concat(L, 2);
                                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                                lua_remove(L, -1);
                                                                                                push_next_close(endObject);
                                                                                                stepDepth('+', 1);
                                                                                                break;
                                                                                        }
                                                                                        break;
                                                                                }
                                                                               break;
                                                                        }
                                                                        break;
                                                                }
                                                                break;
                                                        }
                                                        case JSON_OBJECT_TYPE:
                                                        case JSON_NESTED_OBJECT_TYPE:
                                                        {
                                                                switch(parse_mode)
                                                                {
                                                                        
                                                                        case JSON_ARRAY_TYPE:
                                                                        case JSON_NESTED_ARRAY_TYPE:
                                                                        {
                                                                                const char *key = lua_tostring(L, -1);
                                                                                switch(keytype)
                                                                                {
                                                                                        case LUA_TSTRING:
                                                                                        {
                                                                                                lua_pushstring(L, self->json);
                                                                                                lua_pushfstring(L, newKeyArray, key);
                                                                                                lua_concat(L, 2);
                                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                                lua_remove(L, -1);
                                                                                                push_next_close(endArray);
                                                                                                stepDepth('-', 2);
                                                                                                break;
                                                                                        }
                                                                                        case LUA_TNUMBER:
                                                                                        {
                                                                                                lua_pushstring(L, self->json);
                                                                                                lua_pushfstring(L, newArray);
                                                                                                lua_concat(L, 2);
                                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                                lua_remove(L, -1);
                                                                                                push_next_close(endArray);
                                                                                                stepDepth('-', 2);
                                                                                                break;
                                                                                        }
                                                                                        break;
                                                                                }
                                                                                break;
                                                                        }
                                                                        case JSON_OBJECT_TYPE:
                                                                        case JSON_NESTED_OBJECT_TYPE:
                                                                        {
                                                                                const char *key = lua_tostring(L, -1);
                                                                                
                                                                                switch(keytype)
                                                                                {
                                                                                        case LUA_TSTRING:
                                                                                        { 
                                                                                                lua_pushstring(L, self->json);
                                                                                                lua_pushfstring(L, newKeyObject, key);
                                                                                                lua_concat(L, 2);
                                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                                lua_remove(L, -1);
                                                                                                push_next_close(endObject);
                                                                                                stepDepth('+', 1);   
                                                                                                break;
                                                                                        }
                                                                                        case LUA_TNUMBER:
                                                                                        {
                                                                                                lua_pushstring(L, self->json);
                                                                                                lua_pushfstring(L, newObject);
                                                                                                lua_concat(L, 2);
                                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                                lua_remove(L, -1);
                                                                                                push_next_close(endObject);
                                                                                                stepDepth('+', 1);
                                                                                                break;
                                                                                        }
                                                                                        break;
                                                                                }
                                                                                break;
                                                                        }
                                                                        break;
                                                                }
                                                                break;
                                                        }
                                                        break;
                                                }       
                                                break;
                                        }
                                        break;

                                }
                                stringify(L, -2, self);
                                self->isParent = false;
                                self->isChild = true;
                                break;
                        }
                        /* VAL IS A STRING */              
                        case LUA_TSTRING:
                        case LUA_TBOOLEAN:
                        {
                                parent_type = self->get_type();
                                switch(parent_type)
                                {
                                        case JSON_ARRAY_TYPE:
                                        case JSON_NESTED_ARRAY_TYPE:
                                        {
                                                switch(parse_mode)
                                                {
                                                        case JSON_ARRAY_TYPE:
                                                        {
                                                                const char *key = lua_tostring(L, -1);
                                                                const char *val = (valtype == LUA_TBOOLEAN) ? btoa(lua_toboolean(L, -2)) : lua_tostring(L, -2);

                                                                switch(keytype)
                                                                {
                                                                        case LUA_TSTRING:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(valtype == LUA_TBOOLEAN)
                                                                                        lua_pushfstring(L, nestedkvBool, key, val);
                                                                                else
                                                                                        lua_pushfstring(L, nestedkvString, key, val);

                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        case LUA_TNUMBER:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(valtype == LUA_TBOOLEAN)
                                                                                        lua_pushfstring(L, arrayBool, val);
                                                                                else
                                                                                        lua_pushfstring(L, arrayString, val);

                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;  
                                                                        }
                                                                        break;
                                                                }
                                                                break;
                                                        }
                                                        case JSON_OBJECT_TYPE:
                                                        {
                                                                const char *key = lua_tostring(L, -1);
                                                                const char *val = (valtype == LUA_TBOOLEAN) ? btoa(lua_toboolean(L, -2)) : lua_tostring(L, -2);

                                                                switch(keytype)
                                                                {
                                                                        case LUA_TSTRING:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(parent_type != JSON_OBJECT_TYPE)
                                                                                        if(valtype == LUA_TBOOLEAN)
                                                                                                lua_pushfstring(L, nestedkvBool, key, val);
                                                                                        else
                                                                                                lua_pushfstring(L, nestedkvString, key, val);
                                                                                else
                                                                                        if(valtype == LUA_TBOOLEAN)
                                                                                                lua_pushfstring(L, kvBool, key, val);
                                                                                        else
                                                                                                lua_pushfstring(L, kvString, key, val);

                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        case LUA_TNUMBER:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(valtype == LUA_TBOOLEAN)
                                                                                                lua_pushfstring(L, kvBool, key, val);
                                                                                        else
                                                                                                lua_pushfstring(L, kvString, key, val); 
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;  
                                                                        }
                                                                        break;
                                                                }
                                                                break;
                                                        }
                                                        break;
                                                }
                                                break;
                                        }
                                        case JSON_OBJECT_TYPE:
                                        case JSON_NESTED_OBJECT_TYPE:
                                        {
                                                //parse_mode = parent_type;
                                                switch(parse_mode)
                                                {
                                                        case JSON_ARRAY_TYPE:
                                                        {
                                                                // string key string value in an array in an array [["dick":"shrivells" ** MUST BE NESTED 
                                                                const char *key = lua_tostring(L, -1);
                                                                const char *val = (valtype == LUA_TBOOLEAN) ? btoa(lua_toboolean(L, -2)) : lua_tostring(L, -2);

                                                                switch(keytype)
                                                                {
                                                                        case LUA_TSTRING:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(valtype == LUA_TBOOLEAN)
                                                                                        lua_pushfstring(L, nestedkvBool, key, val);
                                                                                else
                                                                                        lua_pushfstring(L, nestedkvString, key, val); 
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        case LUA_TNUMBER:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(parent_type != JSON_ARRAY_TYPE)
                                                                                        if(valtype == LUA_TBOOLEAN)
                                                                                                lua_pushfstring(L, kvBool, key, val);
                                                                                        else
                                                                                                lua_pushfstring(L, kvString, key, val);
                                                                                else //if(depth < 1 && arrMode)
                                                                                        if(valtype == LUA_TBOOLEAN)
                                                                                                lua_pushfstring(L, arrayBool, val);
                                                                                        else
                                                                                                lua_pushfstring(L, arrayString, val);

                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;  
                                                                        }
                                                                        break;
                                                                }
                                                                break;
                                                        }
                                                        case JSON_OBJECT_TYPE:
                                                        {
                                                                const char *key = lua_tostring(L, -1);
                                                                const char *val = (valtype == LUA_TBOOLEAN) ? btoa(lua_toboolean(L, -2)) : lua_tostring(L, -2);

                                                                switch(keytype)
                                                                {
                                                                        case LUA_TSTRING:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(valtype == LUA_TBOOLEAN)
                                                                                                lua_pushfstring(L, kvBool, key, val);
                                                                                        else
                                                                                                lua_pushfstring(L, kvString, key, val);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        case LUA_TNUMBER:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(valtype == LUA_TBOOLEAN)
                                                                                                lua_pushfstring(L, kvBool, key, val);
                                                                                        else
                                                                                                lua_pushfstring(L, kvString, key, val);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;  
                                                                        }
                                                                        break; 
                                                                }
                                                                break;
                                                        }
                                                        break;
                                                }
                                                break;
                                        }
                                        break;

                                }       
                                self->isParent = false;
                                self->isChild = true;  
                                break;
                        }
                        /* VAL IS A NUMBER */
                        case LUA_TNUMBER:
                        {
                                parent_type = self->get_type();

                                switch(parent_type)
                                {
                                        case JSON_ARRAY_TYPE:
                                        case JSON_NESTED_ARRAY_TYPE:
                                        {
                                                switch(parse_mode)
                                                {
                                                        case JSON_ARRAY_TYPE:
                                                        {
                                                                const char *key = lua_tostring(L, -1);
                                                                lua_Number lnum = lua_tonumber(L, -2);
                                                                
                                                                switch(keytype)
                                                                {
                                                                        case LUA_TSTRING:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, nestedkvNumber, key, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        case LUA_TNUMBER:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, arrayNumber,  lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        break;
                                                                }
                                                                break;
                                                        }
                                                        case JSON_OBJECT_TYPE:
                                                        {
                                                                const char *key = lua_tostring(L, -1);
                                                                lua_Number lnum = lua_tonumber(L, -2);
                                                                
                                                                switch(keytype)
                                                                {
                                                                        case LUA_TSTRING:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, kvNumber, key, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        case LUA_TNUMBER:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, kvNumber, key, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        break;
                                                                }
                                                                break;
                                                        }
                                                        break;
                                                }
                                                break;
                                        }
                                        case JSON_OBJECT_TYPE:
                                        case JSON_NESTED_OBJECT_TYPE:
                                        {
                                                switch(parse_mode)
                                                {
                                                        case JSON_ARRAY_TYPE:
                                                        {
                                                                const char *key = lua_tostring(L, -1);
                                                                lua_Number lnum = lua_tonumber(L, -2);
                                                                
                                                                switch(keytype)
                                                                {
                                                                        case LUA_TSTRING:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                if(parent_type != JSON_ARRAY_TYPE)
                                                                                        lua_pushfstring(L, nestedkvNumber, key, lnum);
                                                                                else
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        case LUA_TNUMBER:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, arrayNumber,  lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                       break;
                                                                }
                                                                break;
                                                        }
                                                        case JSON_OBJECT_TYPE:
                                                        {
                                                                const char *key = lua_tostring(L, -1);
                                                                lua_Number lnum = lua_tonumber(L, -2);

                                                                switch(keytype)
                                                                {
                                                                        case LUA_TSTRING:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, kvNumber, key, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        case LUA_TNUMBER:
                                                                        {
                                                                                lua_pushstring(L, self->json);
                                                                                lua_pushfstring(L, kvNumber, key, lnum);
                                                                                lua_concat(L, 2);
                                                                                strcpy(self->json, lua_tostring(L, -1));
                                                                                lua_remove(L, -1);
                                                                                break;
                                                                        }
                                                                        break;
                                                                }
                                                                break;
                                                        }
                                                        break;
                                                }
                                                break;
                                        }
                                        break;
                                }       
                                break;
                                        
                        }
                        case LUA_TFUNCTION:
                        case LUA_TUSERDATA:
                        case LUA_TLIGHTUSERDATA:
                        case LUA_TNONE:
                        case LUA_TTHREAD:
                        case LUA_TNIL:
                        {
                                self->unSuppoted = true;
                                break;
                        }                      
                        break; 
                }
                self->isParent = false;
                self->isChild = true;  
                // pop value + copy of key, leaving original key
                lua_pop(L, 2);
                // stack now contains: -1 => key; -2 => table
        } /* LOOP */


        switch(self->ttype)
        {
                case JSON_ARRAY_TYPE:
                {
                        lua_pushstring(L, self->json);
                        lua_pushstring(L, get_next_close());
                        lua_concat(L, 2);
                        strcpy(self->json, lua_tostring(L, -1));
                        lua_remove(L, -1);
                        pop_next_close();
                        if(depth > 0) depth--;
                        if(nc_cnt == 1 && depth == 0)
                        {
                                parent_type = get_parent_type();
                                parse_mode = self->get_type();
                        }
                        break;
                }
                case JSON_OBJECT_TYPE:
                {
                        lua_pushstring(L, self->json);
                        lua_pushstring(L, get_next_close());
                        lua_concat(L, 2);
                        strcpy(self->json, lua_tostring(L, -1));
                        lua_remove(L, -1);
                        pop_next_close();
                        if(depth > 0) depth--;
                        if(nc_cnt == 1 && depth == 0)
                        {
                                parent_type = get_parent_type();
                                parse_mode = self->get_type();
                        }
                        break;
                }
                break;
        }
        if(nc_cnt <= 0)
        {
                lua_pushstring(L, self->json);
                lua_pushstring(L, get_root_close());
                lua_concat(L, 2);
                strcpy(self->json, lua_tostring(L, -1));
                lua_remove(L, -1);
                pop_root_close();
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
static int _lua_parse_json(lua_State *L, struct mg_str json, int depth)
{
        if(isRoot || depth == 0)
                isRoot = false;

        int idx = 1;
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
                                _lua_parse_json(L, val, depth += 2);

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
        pL.get_type = get_parent_type;
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
        _lua_parse_json(L, json, depth);
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
