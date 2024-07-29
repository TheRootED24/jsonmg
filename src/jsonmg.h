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


const char *str1 = "{\"some\":{\"json\":\"json2\",\"number\": 100,\"test\":\"testycle\"}}";
const char *arrA = "[1,2,3,4,5,\"test\",{\"testy\":{\"joe\":\"woods\"}},{\"frank\":\"black\"}]";
const char *kv = "{\"key\":\"val\",\"foo\":\"bar\", \"num\": 100, \"test\":{\"joe\":\"woods\"}, \"pstate\":[1,2,3,4,5,6]}";
const char *str10 = "{\"system\":{\"get_SysInfo\":{\"sw_ver\":\"1.0.5 Build 230613 Rel.151643\",\"hw_ver\":\"4.0\",\"model\":\"KL125(US)\",\"deviceId\":\"80125C8762B7CF2F90B3852FFCB0775B21C71C00\",\"oemId\":\"82E6C9A57FC634FA41A9A8D339B1A6D4\",\"hwId\":\"F5DFE24039B182A525EA715C41177D2A\",\"rssi\":-50,\"latitude_i\":441817,\"longitude_i\":-816185,\"alias\":\"lamp backroom\",\"status\":\"new\",\"obd_src\":\"tplink\",\"description\":\"Smart Wi-Fi LED Bulb with Color Changing\",\"mic_TYPE\":\"IOT.SMARTBULB\",\"mic_mac\":\"5CE93173AC2A\",\"dev_state\":\"normal\",\"is_factory\":\"true\",\"disco_ver\":\"1.0\",\"ctrl_protocols\":{\"name\":\"Linkie\",\"version\":\"1.0\"},\"active_mode\":\"none\",\"is_dimmable\":1,\"is_color\":1,\"is_variable_color_temp\":1,\"light_state\":{\"on_off\":0,\"dft_on_state\":{\"mode\":\"normal\",\"hue\":240,\"saturation\":100,\"color_temp\":0,\"brightness\":100}},\"preferred_state\":[{\"index\":0,\"hue\":0,\"saturation\":0,\"color_temp\":2700,\"brightness\":100},{\"index\":1,\"hue\":0,\"saturation\":100,\"color_temp\":0,\"brightness\":100},{\"index\":2,\"hue\":120,\"saturation\":100,\"color_temp\":0,\"brightness\":100},{\"index\":3,\"hue\":0,\"saturation\":100,\"color_temp\":0,\"brightness\":50}],\"err_code\":0}}}";
const char *str11 = "{\"sysinfo\":{\"system\":{\"get_SysInfo\":{\"sw_ver\":\"1.0.8 Build 230804 Rel.172306\",\"hw_ver\":\"2.0\",\"model\":\"KP303(US)\",\"deviceId\":\"8006BA35E8278C168136D970EE56DE201E8B7EE0\",\"oemId\":\"3DB05524283BBF3A7D248CBB9389029D\",\"hwId\":\"9FD90762FBE6CD307B19E70B5EE345E2\",\"rssi\":-46,\"latitude_i\":-1879048193,\"longitude_i\":-1879048193,\"alias\":\"TP-LINK_Power Strip_0A7D\",\"status\":\"new\",\"obd_src\":\"tplink\",\"mic_TYPE\":\"IOT.SMARTPLUGSWITCH\",\"feature\":\"TIM\",\"mac\":\"60:A4:B7:06:0A:7D\",\"updating\":0,\"led_off\":0,\"children\":[{\"id\":\"8006BA35E8278C168136D970EE56DE201E8B7EE000\",\"state\":1,\"alias\":\"EXHAUST FAN\",\"on_time\":1,\"next_action\":{\"parent_type\":1,\"schd_sec\":82800,\"action\":0}},{\"id\":\"8006BA35E8278C168136D970EE56DE201E8B7EE001\",\"state\":0,\"alias\":\"Light1\",\"on_time\":1,\"next_action\":{\"parent_type\":1,\"schd_sec\":82800,\"action\":0}},{\"id\":\"8006BA35E8278C168136D970EE56DE201E8B7EE002\",\"state\":1,\"alias\":\"HUMIDIFIER\",\"on_time\":2,\"next_action\":{\"parent_type\":1,\"schd_sec\":82800,\"action\":0}}],\"child_num\":3,\"ntc_state\":0,\"err_code\":0}}}, \"ip\":\"10.0.10.174\"}";


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


char jstr[MAX_JSON_LEN] = {0};

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
