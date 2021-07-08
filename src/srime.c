#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rime_api.h>

#include "srime.h"

const char *RIME_SHARED_DATA_DIR = "/usr/share/rime-data";

static char *copy_string(char *str) {
  if (str) {
    size_t size = strlen(str);
    char *new_str = malloc(size * sizeof(char));
    strcpy(new_str, str);
    return new_str;
  } else {
    return NULL;
  }
}

static char *get_xdg_data_home() {
  const char *xdg_data_home = getenv("XDG_DATA_HOME");
  if (xdg_data_home) {
    return strdup(xdg_data_home);
  } else {
    const char *home = getenv("HOME");
    char *path = malloc(strlen(home) + strlen("/.local/share") + 1);
    strcpy(path, home);
    strcat(path, "/.local/share");
    return path;
  }
}

static char *get_user_data_dir() {
  char *xdg_data_home = get_xdg_data_home();
  char *path = malloc(strlen(xdg_data_home) + strlen(LUARIME_NAME) + 2);
  strcpy(path, xdg_data_home);
  strcat(path, "/");
  strcat(path, LUARIME_NAME);
  free(xdg_data_home);
  return path;
}

unsigned long rime_initial() {
  RIME_STRUCT(RimeTraits, lua_rime_traits);

  char *user_data_dir = get_user_data_dir();
  lua_rime_traits.shared_data_dir = RIME_SHARED_DATA_DIR;
  lua_rime_traits.app_name = "rime.lua";
  lua_rime_traits.user_data_dir = user_data_dir;
  lua_rime_traits.distribution_name = "Rime";
  lua_rime_traits.distribution_code_name = "luarime";
  lua_rime_traits.distribution_version = "1.0.0";

  RimeSetup(&lua_rime_traits);
  printf("id\n");
  RimeInitialize(&lua_rime_traits);
  printf("init id\n");
  free(user_data_dir);
  RimeStartMaintenance(true);
  RimeJoinMaintenanceThread();

  RimeSessionId session_id = RimeCreateSession();

  return session_id;
}

int rime_finalize(unsigned long session_id) {
  RimeDestroySession(session_id);
  RimeFinalize();
  return 1;
}

int rime_get_option(unsigned long session_id, const char *option) {
  return RimeGetOption(session_id, option);
}

void rime_set_option(unsigned long session_id, const char *option, int value) {
  RimeSetOption(session_id, option, value);
}

void free_lua_rime_result(LuaRimeResult *result) {
  if (result->commit_text_preview) {
    free(result->commit_text_preview);
  }
  if (result->composition_preedit) {
    free(result->composition_preedit);
  }
  if (result->candidates) {
    for (int i = 0; i < result->num_candidates; i++) {
      free(result->candidates[i]);
    }
    free(result->candidates);
  }
}

void init_lua_rime_result(LuaRimeResult *result) {
  result->commit_text_preview = NULL;
  result->composition_preedit = NULL;
  result->composition_length = 0;
  result->composition_cursor_pos = 0;
  result->composition_sel_start = 0;
  result->composition_sel_end = 0;
  result->num_candidates = 0;
  result->candidates = NULL;
  result->page_no = 0;
  result->page_size = 0;
}

/**
 * keycode
 * mask => modifiers shift   1 ; 1 << 0
 *                   control 4 ; 1 << 2
 *                   meta    8 ; 1 << 3
 **/
int rime_process_key(unsigned long session_id, int keycode, int mask) {
  if (RimeFindSession(session_id)) {
    int status = RimeProcessKey(session_id, keycode, mask);
    return status;
  }
  return 0;
}

int rime_context(LuaRimeResult *result, unsigned long session_id) {
  init_lua_rime_result(result);
  RIME_STRUCT(RimeContext, context);
  if (RimeGetContext(session_id, &context)) {
    if (context.commit_text_preview) {
      result->commit_text_preview = copy_string(context.commit_text_preview);
    }

    result->composition_length = context.composition.length;
    result->composition_cursor_pos = context.composition.cursor_pos;
    result->composition_sel_start = context.composition.sel_start;
    result->composition_sel_end = context.composition.sel_end;
    if (context.composition.preedit) {
      result->composition_preedit = copy_string(context.composition.preedit);
    }

    if (context.menu.num_candidates) {
      result->num_candidates = context.menu.num_candidates;
      char **candidates =
          (char **)malloc(context.menu.num_candidates * sizeof(char *));
      int i;
      for (i = 0; i < context.menu.num_candidates; i++) {
        candidates[i] = copy_string(context.menu.candidates[i].text);
      }
      result->candidates = candidates;
      result->page_no = context.menu.page_no;
      result->page_size = context.menu.page_size;
    }
    RimeFreeContext(&context);
    return 1;
  }
  return 0;
}

char *rime_get_commit(unsigned long session_id) {
  RIME_STRUCT(RimeCommit, commit);
  if (RimeGetCommit(session_id, &commit)) {
    char *commit_str = copy_string(commit.text);
    RimeFreeCommit(&commit);
    return commit_str;
  }
  return NULL;
}

void rime_clear_composition(unsigned long session_id) {
  RimeClearComposition(session_id);
}

int rime_select_schema(unsigned long session_id, const char *schema_id) {
  return RimeSelectSchema(session_id, schema_id);
}

int rime_get_schema_list(unsigned long session_id) {
  RimeSchemaList schema_list;

  if (RimeGetSchemaList(&schema_list)) {
    for (int i = 0; i < schema_list.size; i++) {
      RimeSchemaListItem item = schema_list.list[i];
      printf("%s , %s\n", item.schema_id, item.name);
    }
    RimeFreeSchemaList(&schema_list);
  }
  return 1;
}

char *rime_get_version() {
  RimeApi *rime_api = rime_get_api();
  char *version  = copy_string((char *)rime_api->get_version());
  rime_api->finalize();
  return version;
}
