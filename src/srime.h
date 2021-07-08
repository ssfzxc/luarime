#ifndef SRIME_H_
#define SRIME_H_

#ifndef LUARIME_NAME
#define LUARIME_NAME "lua-rime"
#endif

#ifndef LUARIME_VERSION
#define LUARIME_VERSION "1.0.0"
#endif

typedef struct _LuaRimeResult {
  char *commit_text_preview;
  char *composition_preedit;
  int composition_length;
  int composition_cursor_pos;
  int composition_sel_start;
  int composition_sel_end;
  char **candidates;
  int num_candidates;
  int page_no;
  int page_size;
} LuaRimeResult;

extern void free_lua_rime_result(LuaRimeResult *result);
extern void init_lua_rime_result(LuaRimeResult *result);

extern unsigned long rime_initial();
extern int rime_finalize(unsigned long session_id);
extern int rime_get_option(unsigned long session_id, const char *option);
extern void rime_set_option(unsigned long session_id, const char *option, int value);
extern int rime_process_key(unsigned long session_id, int keycode, int mask);
extern int rime_context(LuaRimeResult *result, unsigned long session_id);
extern char *rime_get_commit(unsigned long session_id);
extern int rime_select_schema(unsigned long session_id, const char *schema_id);
extern int rime_get_schema_list(unsigned long session_id);
extern char *rime_get_version();
#endif
