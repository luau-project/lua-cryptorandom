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
#include <Security/Security.h>
#elif defined(LUA_CRYPTORANDOM_USE_OPENSSL)
#include <openssl/rand.h>
#include <openssl/err.h>
#endif

#include <stdlib.h>
#include <luaconf.h>
#include <lauxlib.h>
#include <lualib.h>

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
    if ((*err = SecRandomCopyBytes(kSecRandomDefault, len, buffer)) != errSecSuccess)
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

static int lua_cryptorandom_bytes(lua_State *L)
{
    /*
    ** length of the buffer used to
    ** fill with random bytes
    */
    int len = (int)luaL_checkinteger(L, 1);
    
    luaL_argcheck(L, len > 0, 1, "length must be a positive integer");

    unsigned char *buffer = (unsigned char *)malloc((unsigned int)len);
    if (buffer == NULL)
    {
        luaL_error(L, "Memory allocation failed");
    }

    unsigned long err;
    if (lua_cryptorandom_bytes_impl(L, buffer, len, &err) == 0)
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

    free(buffer);

    return 2;
}

static int lua_cryptorandom_seed(lua_State *L)
{
    unsigned char buffer[sizeof(int)];

    unsigned long err;
    if (lua_cryptorandom_bytes_impl(L, (unsigned char *)buffer, sizeof(int), &err) == 0)
    {
        lua_pushnil(L);
        lua_pushinteger(L, (lua_Integer)err);
    }
    else
    {
        int *seed_ptr = (int *)buffer;
        lua_pushinteger(L, *seed_ptr);
        lua_pushnil(L);
    }

    return 2;
}

static int lua_cryptorandom_integer(lua_State *L)
{
    unsigned char buffer[sizeof(lua_Integer)];

    unsigned long err;
    if (lua_cryptorandom_bytes_impl(L, (unsigned char *)buffer, sizeof(lua_Integer), &err) == 0)
    {
        lua_pushnil(L);
        lua_pushinteger(L, (lua_Integer)err);
    }
    else
    {
        lua_Integer *seed_ptr = (lua_Integer *)buffer;
        lua_pushinteger(L, *seed_ptr);
        lua_pushnil(L);
    }

    return 2;
}

static int lua_cryptorandom_number(lua_State *L)
{
    unsigned char buffer[sizeof(lua_Number)];

    unsigned long err;
    if (lua_cryptorandom_bytes_impl(L, (unsigned char *)buffer, sizeof(lua_Number), &err) == 0)
    {
        lua_pushnil(L);
        lua_pushinteger(L, (lua_Integer)err);
    }
    else
    {
        lua_Number *seed_ptr = (lua_Number *)buffer;
        lua_pushnumber(L, *seed_ptr);
        lua_pushnil(L);
    }

    return 2;
}

static int lua_cryptorandom_new_index(lua_State *L)
{
    luaL_error(L, "Read-only object");
    return 1;
}

static const luaL_Reg lua_cryptorandom_public_functions[] = {
    {"bytes", lua_cryptorandom_bytes},
    {"seed", lua_cryptorandom_seed},
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