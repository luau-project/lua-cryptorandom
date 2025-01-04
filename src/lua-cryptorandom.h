#ifndef LUA_CRYPTORANDOM_H
#define LUA_CRYPTORANDOM_H

#include <lua.h>

#ifdef LUA_CRYPTORANDOM_BUILD_STATIC
#define LUA_CRYPTORANDOM_EXPORT
#else
#ifdef LUA_CRYPTORANDOM_BUILD_SHARED /* { */
#if defined(_WIN32) /* { */
#if defined(__GNUC__) || defined(__MINGW32__) /* { */
#define LUA_CRYPTORANDOM_EXPORT __attribute__((dllexport))
#else /* }{ */
#define LUA_CRYPTORANDOM_EXPORT __declspec(dllexport)
#endif /* } */
#else /* }{ */
#define LUA_CRYPTORANDOM_EXPORT __attribute__((visibility("default")))
#endif /* } */
#else /* }{ */
#if defined(_WIN32) /* { */
#if defined(__GNUC__) || defined(__MINGW32__) /* { */
#define LUA_CRYPTORANDOM_EXPORT __attribute__((dllimport))
#else /* }{ */
#define LUA_CRYPTORANDOM_EXPORT __declspec(dllimport)
#endif /* } */
#else /* }{ */
#define LUA_CRYPTORANDOM_EXPORT
#endif /* } */
#endif /* } */
#endif

#ifdef __cplusplus
extern "C" {
#endif

LUA_CRYPTORANDOM_EXPORT int luaopen_cryptorandom(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif