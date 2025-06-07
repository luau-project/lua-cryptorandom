#include "lua-cryptorandom.h"

#if defined(LUA_CRYPTORANDOM_USE_WIN32)
#include <ntstatus.h>
#ifndef WIN32_NO_STATUS
#define WIN32_NO_STATUS
#endif
#include <windows.h>
#ifdef WIN32_NO_STATUS
#undef WIN32_NO_STATUS
#endif
#include <bcrypt.h>
#elif defined(LUA_CRYPTORANDOM_USE_APPLE)
#include <CommonCrypto/CommonRandom.h>
#elif defined(LUA_CRYPTORANDOM_USE_OPENSSL)
#include <openssl/rand.h>
#include <openssl/err.h>
#endif

#include <math.h>

#include <luaconf.h>
#include <lauxlib.h>
#include <lualib.h>

#ifndef isnan
#define isnan(x) (!((x)==(x)))
#endif

/*
** On Lua 5.3 and newer, the Lua interpreter
** can be built using the type 'lua_Number' as 'float'
** or 'long double'. So, try to account for this
** choice. The default configuration ('lua_Number' as 'double')
** will try to use 'isinf' from <math.h> when the system
** provided it.
*/
#if ((LUA_VERSION_NUM > 502) && (defined(LUA_FLOAT_TYPE)) && (defined(LUA_FLOAT_FLOAT)) && (LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT) && defined(HUGE_VALF))
#define lua_cryptorandom_isinf(x) ((x)==(HUGE_VALF)?(1):((x)==(-HUGE_VALF)?(-1):(0)))
#elif ((LUA_VERSION_NUM > 502) && (defined(LUA_FLOAT_TYPE)) && (defined(LUA_FLOAT_LONGDOUBLE)) && (LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE) && defined(HUGE_VALL))
#define lua_cryptorandom_isinf(x) ((x)==(HUGE_VALL)?(1):((x)==(-HUGE_VALL)?(-1):(0)))
#elif (defined(isinf)) /* on the usual configuration such that lua_Number is double, use isinf in case the system provides it */
#define lua_cryptorandom_isinf isinf
#else
#define lua_cryptorandom_isinf(x) ((x)==(HUGE_VAL)?(1):((x)==(-HUGE_VAL)?(-1):(0)))
#endif

/*
** helper union to the function
** lua_cryptorandom_take
*/
typedef union tagLuaCryptoRandomInt
{
    int value;
    unsigned char buffer[sizeof(int)];

} LuaCryptoRandomInt;

/*
** helper union to the function
** lua_cryptorandom_integer
*/
typedef union tagLuaCryptoRandomInteger
{
    lua_Integer value;
    unsigned char buffer[sizeof(lua_Integer)];

} LuaCryptoRandomInteger;

/*
** helper union to the function
** lua_cryptorandom_number
*/
typedef union tagLuaCryptoRandomNumber
{
    lua_Number value;
    unsigned char buffer[sizeof(lua_Number)];

} LuaCryptoRandomNumber;

#define LUA_CRYPTORANDOM_METATABLE "lua_cryptorandom_metatable"

/*  Implementation to generate cryptographically secure pseudo random bytes */
static int lua_cryptorandom_bytes_impl(lua_State *L, unsigned char *buffer, int len, unsigned long *err)
{
    int result = 1;
#ifdef LUA_CRYPTORANDOM_USE_WIN32
    if ((*err = BCryptGenRandom(NULL, buffer, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG)) != STATUS_SUCCESS)
    {
        result = 0;
    }

#elif defined(LUA_CRYPTORANDOM_USE_APPLE)
    if ((*err = CCRandomGenerateBytes((void *)buffer, len)) != kCCSuccess)
    {
        result = 0;
    }

#elif defined(LUA_CRYPTORANDOM_USE_OPENSSL)
    if (RAND_bytes(buffer, len) != 1)
    {
        *err = ERR_get_error();
        result = 0;
    }
#endif

    return result;
}

/*
** generate 'n' random bytes
*/
static int lua_cryptorandom_bytes(lua_State *L)
{
    void *ud;
    lua_Alloc allocf;
    size_t buffer_size;
    unsigned long err;
    unsigned char *buffer;

    /*
    ** length of the buffer used to
    ** fill with random bytes
    */
    int len = (int)luaL_checkinteger(L, 1);

    luaL_argcheck(L, len > 0, 1, "length must be a positive integer");

    buffer_size = ((size_t)len) * sizeof(unsigned char);

    /*
    ** underpromotes buffer_size to 'int', then
    ** check if buffer_size as 'int' has the same value
    ** as buffer_size as 'size_t'
    */
    luaL_argcheck(L, buffer_size == ((size_t)((int)buffer_size)), 1, "length is too long");

    /* gets Lua's memory allocator function */
    allocf = lua_getallocf(L, &ud);

    /* allocate the buffer through Lua's memory allocator */
    buffer = (unsigned char *)allocf(ud, NULL, 0, buffer_size);

    /* memory allocation failed? */
    if (buffer == NULL)
    {
        luaL_error(L, "Memory allocation failed");
    }

    if (lua_cryptorandom_bytes_impl(L, buffer, ((int)buffer_size), &err) == 0)
    {
        lua_pushnil(L);
        lua_pushinteger(L, (lua_Integer)err);
    }
    else
    {
        lua_createtable(L, len, 0);
        for (int i = 1; i <= len; i++)
        {
            lua_pushinteger(L, i);
            lua_pushinteger(L, 0xFF & (buffer[i - 1]));
            lua_settable(L, -3);
        }
        lua_pushnil(L);
    }

    /* free the buffer through Lua's memory allocator */
    allocf(ud, (void *)buffer, buffer_size, 0);

    return 2;
}

/*
** generate a random 'int'
*/
static int lua_cryptorandom_take(lua_State *L)
{
    LuaCryptoRandomInt rtake;

    unsigned long err;
    if (lua_cryptorandom_bytes_impl(L, (unsigned char *)(rtake.buffer), sizeof(int) * sizeof(unsigned char), &err) == 0)
    {
        lua_pushnil(L);
        lua_pushinteger(L, (lua_Integer)err);
    }
    else
    {
        lua_pushinteger(L, rtake.value);
        lua_pushnil(L);
    }

    return 2;
}

/*
** generate a random 'lua_Integer'
*/
static int lua_cryptorandom_integer(lua_State *L)
{
    LuaCryptoRandomInteger rint;

    unsigned long err;
    if (lua_cryptorandom_bytes_impl(L, (unsigned char *)(rint.buffer), sizeof(lua_Integer) * sizeof(unsigned char), &err) == 0)
    {
        lua_pushnil(L);
        lua_pushinteger(L, (lua_Integer)err);
    }
    else
    {
        lua_pushinteger(L, rint.value);
        lua_pushnil(L);
    }

    return 2;
}

/*
** generate a random 'lua_Number'
** 
** note: 'NaN' or '+- inf' is NOT
**       allowed as the generated value.
*/
static int lua_cryptorandom_number(lua_State *L)
{
    LuaCryptoRandomNumber rnum;
    unsigned long err;

    int is_nan_or_inf = 1;
    int had_error = 0;

    while (!had_error && is_nan_or_inf)
    {
        if (lua_cryptorandom_bytes_impl(L, (unsigned char *)(rnum.buffer), sizeof(lua_Number) * sizeof(unsigned char), &err) == 0)
        {
            lua_pushnil(L);
            lua_pushinteger(L, (lua_Integer)err);
            had_error = 1;
        }
        else if (!(isnan(rnum.value) || lua_cryptorandom_isinf(rnum.value)))
        {
            lua_pushnumber(L, rnum.value);
            lua_pushnil(L);
            is_nan_or_inf = 0;
        }
    }
    
    return 2;
}

static int lua_cryptorandom_new_index(lua_State *L)
{
    return luaL_error(L, "Read-only object");
}

static const luaL_Reg lua_cryptorandom_public_functions[] = {
    {"bytes", lua_cryptorandom_bytes},
    {"take", lua_cryptorandom_take},
    {"integer", lua_cryptorandom_integer},
    {"number", lua_cryptorandom_number},
    {NULL, NULL}
};

LUA_CRYPTORANDOM_EXPORT int luaopen_cryptorandom(lua_State *L)
{
    lua_createtable(L, 0, 0);

    luaL_newmetatable(L, LUA_CRYPTORANDOM_METATABLE);

#if LUA_VERSION_NUM == 501
    luaL_register(L, NULL, lua_cryptorandom_public_functions);
#else
    luaL_setfuncs(L, lua_cryptorandom_public_functions, 0);
#endif

    lua_pushstring(L, "version");
    lua_pushstring(L, LUA_CRYPTORANDOM_VERSION);
    lua_settable(L, -3);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    lua_pushstring(L, "__metatable");
    lua_pushboolean(L, 0);
    lua_settable(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, lua_cryptorandom_new_index);
    lua_settable(L, -3);

    lua_setmetatable(L, -2);

    return 1;
}
