#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srime.h"
#include <lauxlib.h>
#include <lua.h>

static void ultoa(unsigned long id, char *sid) { sprintf(sid, "%lu", id); }

static unsigned long atoul(const char *sid) {
  unsigned long id;
  sscanf(sid, "%lu", &id);
  return id;
}

static int luarime_initial(lua_State *l) {
  printf("Start\n");
  unsigned long id = rime_initial();
  printf("id: %lu\n", id);
  char sid[20];
  ultoa(id, sid);
  lua_pushstring(l, sid);
  return 1;
}

static unsigned long get_session_id(lua_State *l) {
  const char *sid = luaL_checkstring(l, 1);
  return atoul(sid);
}

static int luarime_finalize(lua_State *l) {
  unsigned long id = get_session_id(l);
  return rime_finalize(id);
}

static int luarime_get_option(lua_State *l) {
  unsigned long id = get_session_id(l);
  const char *option = lua_tostring(l, 2);
  return rime_get_option(id, option);
}

static int luarime_set_option(lua_State *l) {
  unsigned long id = get_session_id(l);
  const char *option = lua_tostring(l, 2);
  const int value = lua_toboolean(l, 3);
  rime_set_option(id, option, value);
  return 1;
}

static int luarime_process_key(lua_State *l) {
  unsigned long id = get_session_id(l);
  const int keycode = luaL_checkinteger(l, 2);
  const int mask = luaL_checkinteger(l, 3);
  return rime_process_key(id, keycode, mask);
}

static int luarime_context(lua_State *l) {
  unsigned long id = get_session_id(l);
  LuaRimeResult result;
  int status = rime_context(&result, id);
  lua_newtable(l);
  lua_pushstring(l, "commit_text_preview");
  lua_pushstring(l, result.commit_text_preview);
  lua_settable(l, -3);

  lua_pushstring(l, "composition_preedit");
  lua_pushstring(l, result.composition_preedit);
  lua_settable(l, -3);

  lua_pushstring(l, "composition_length");
  lua_pushinteger(l, result.composition_length);
  lua_settable(l, -3);

  lua_pushstring(l, "composition_cursor_pos");
  lua_pushinteger(l, result.composition_cursor_pos);
  lua_settable(l, -3);

  lua_pushstring(l, "composition_sel_start");
  lua_pushinteger(l, result.composition_sel_start);
  lua_settable(l, -3);

  lua_pushstring(l, "composition_sel_end");
  lua_pushinteger(l, result.composition_sel_end);
  lua_settable(l, -3);

  lua_pushstring(l, "num_candidates");
  lua_pushinteger(l, result.num_candidates);
  lua_settable(l, -3);

  lua_pushstring(l, "page_no");
  lua_pushinteger(l, result.page_no);
  lua_settable(l, -3);

  lua_pushstring(l, "page_size");
  lua_pushinteger(l, result.page_size);
  lua_settable(l, -3);

  for (int i = 0; i < result.num_candidates; i++) {
    lua_pushinteger(l, i + 1);
    lua_pushstring(l, result.candidates[i]);
    lua_settable(l, -3);
  }
  free_lua_rime_result(&result);
  return 1;
}

static int luarime_get_commit(lua_State *l) {
  unsigned long id = get_session_id(l);
  char *str = rime_get_commit(id);
  lua_pushstring(l, str);
  free(str);
  return 1;
}

/**
extern int rime_select_schema(unsigned long session_id, const char *schema_id);
extern int rime_get_schema_list(unsigned long session_id);
**/

static int luarime_version(lua_State *l) {
  char *version = rime_get_version();
  lua_pushstring(l, version);
  free(version);
  return 1;
}

/* ===== INITIALISATION ===== */

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502

static void luaL_setfuncs(lua_State *l, const luaL_Reg *reg, int nup) {
  int i;

  luaL_checkstack(l, nup, "too many upvalues");
  for (; reg->name != NULL; reg++) { /* fill the table with given functions */
    for (i = 0; i < nup; i++) /* copy upvalues to the top */
      lua_pushvalue(l, -nup);
    lua_pushcclosure(l, reg->func, nup); /* closure with those upvalues */
    lua_setfield(l, -(nup + 2), reg->name);
  }
  lua_pop(l, nup); /* remove upvalues */
}

#define luaL_newlib(L,l)  \
  (lua_newtable(L), luaL_setfuncs(L,l,0))

#endif

static int lua_rime_new(lua_State *l) {
  luaL_Reg reg[] = {{"version", luarime_version},
                    {"initial", luarime_initial},
                    {"finalize", luarime_finalize},
                    {"process_key", luarime_process_key},
                    {"context", luarime_context},
                    {"get_commit", luarime_get_commit},
                    {"get_option", luarime_get_option},
                    {"set_option", luarime_set_option},
                    {NULL, NULL}};

  luaL_newlib(l, reg);

  return 1;
}

extern int luaopen_libluarime(lua_State *l) {
  lua_rime_new(l);
  return 1;
}
