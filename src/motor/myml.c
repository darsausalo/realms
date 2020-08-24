#include "motor.h"
#include <setjmp.h>

static myml_error_t no_error = {.line = 0, .column = 0, .message = NULL};

// TODO: support utf8

// TODO: move to common lib
static char* myml_strdup(const char* s) {
  size_t len = strlen(s);
  char* p = malloc(len + 1);
  if (p) {
    memcpy(p, s, len);
    p[len] = 0;
  }
  return p;
}

// TODO: move to common lib
static char* myml_strndup(const char* s, size_t n) {
  size_t len = strnlen(s, n);
  char* p = malloc(len + 1);
  if (p) {
    memcpy(p, s, len);
    p[len] = 0;
  }
  return p;
}

typedef enum { MVT_STRING, MVT_TABLE } myml_value_type_t;

struct myml_value_t {
  myml_value_type_t type;
  const char* key;
  union {
    const char* string;
    myml_table_t* table;
  };
  int line;
  int column;
};

struct myml_table_t {
  myml_value_t* values;
  int size;
  // TODO: use capacity for myml_table_t
};

typedef struct myml_stack_t {
  myml_table_t** tables;
  int size;
  int capacity;
} myml_stack_t;

static void myml_stack_push(myml_stack_t* stack, myml_table_t* table) {
  int index = stack->size;
  stack->size++;
  if (stack->size >= stack->capacity) {
    stack->capacity = stack->capacity != 0 ? stack->capacity * 2 : 4;
    stack->tables =
        realloc(stack->tables, stack->capacity * sizeof(myml_table_t));
  }
  stack->tables[index] = table;
}

static void myml_stack_pop(myml_stack_t* stack) {
  if (stack->size > 0) { stack->size--; }
}

static myml_table_t* myml_stack_top(const myml_stack_t* stack) {
  return stack->tables && stack->size > 0 ? stack->tables[stack->size - 1]
                                          : NULL;
}

typedef struct myml_parse_context_t {
  int line;
  int column;
  myml_stack_t levels;
} myml_parse_context_t;

static myml_parse_result_t myml_error(myml_parse_context_t* ctx,
                                      const char* message) {
  myml_parse_result_t result = {
      .table = NULL,
      .error = {.line = ctx->line, .column = ctx->column, message = message}};
  return result;
}

static char* myml_sanitize_value(const char* value, int len) {
  if (!value) return NULL;

  const char* sanitized_value = value;
  while (*sanitized_value == ' ') sanitized_value++;
  len -= (int) (sanitized_value - value);
  if (len <= 0) return NULL;

  return myml_strndup(sanitized_value, len);
}

myml_table_t* myml_alloc() { return calloc(1, sizeof(myml_table_t)); }

void myml_free(myml_table_t* table) {
  if (!table) return;

  for (int i = 0; i < table->size; i++) {
    myml_value_t* value = &table->values[i];
    if (value->type == MVT_TABLE) {
      myml_free(value->table);
    } else {
      free((void*) (intptr_t) value->string);
    }
    free((void*) (intptr_t) value->key);
  }

  free((void*) (intptr_t) table);
}

myml_value_t* myml_get_value(myml_table_t* table, const char* key) {
  if (!table || !table->values) return NULL;

  for (int i = 0; i < table->size; i++) {
    if (strcmp(table->values[i].key, key) == 0) return &table->values[i];
  }

  return NULL;
}

const char* myml_get_string(myml_table_t* table, const char* key) {
  myml_value_t* value = myml_get_value(table, key);
  return value && value->type == MVT_STRING ? value->string : NULL;
}

myml_table_t* myml_get_subtable(myml_table_t* table, const char* key) {
  myml_value_t* value = myml_get_value(table, key);
  return value && value->type == MVT_TABLE ? value->table : NULL;
}

void myml_get_location(myml_value_t* value, int* line, int* column) {
  *line = value->line;
  *column = value->column;
}

const char* myml_find_string(myml_table_t* table, const char* key_path) {
  int len = 0;
  while (key_path[len] != '\0' && key_path[len] != '.') len++;
  bool partial = key_path[len] == '.';

  for (int i = 0; i < table->size; i++) {
    myml_value_t* value = &table->values[i];
    if (strncmp(value->key, key_path, len) == 0) {
      if (partial && value->type == MVT_TABLE) {
        return myml_find_string(value->table, &key_path[len + 1]);
      } else if (!partial && value->type == MVT_STRING) {
        return value->string;
      }
    }
  }

  return NULL;
}

myml_table_t* myml_find_subtable(myml_table_t* table, const char* key_path) {
  int len = 0;
  while (key_path[len] != '\0' && key_path[len] != '.') len++;
  bool partial = key_path[len] == '.';

  for (int i = 0; i < table->size; i++) {
    myml_value_t* value = &table->values[i];
    if (strncmp(value->key, key_path, len) == 0 && value->type == MVT_TABLE) {
      return partial ? myml_find_subtable(value->table, &key_path[len + 1])
                     : value->table;
    }
  }

  return NULL;
}

static myml_value_t* myml_set_string_nocopy(myml_table_t* table,
                                            const char* key,
                                            const char* string) {
  myml_value_t* value = myml_get_value(table, key);
  if (!value) {
    table->size++;
    table->values = realloc(table->values, table->size * sizeof(myml_value_t));
    value = &table->values[table->size - 1];
    value->key = myml_strdup(key);
  }
  value->type = MVT_STRING;
  value->string = string;

  return value;
}

myml_value_t* myml_set_string(myml_table_t* table, const char* key,
                              const char* string, size_t len) {
  myml_value_t* value = myml_get_value(table, key);
  if (!value) {
    table->size++;
    table->values = realloc(table->values, table->size * sizeof(myml_value_t));
    value = &table->values[table->size - 1];
    value->key = myml_strdup(key);
  }
  value->type = MVT_STRING;
  value->string = myml_strndup(string, len);

  return value;
}

myml_value_t* myml_set_subtable(myml_table_t* table, const char* key,
                                myml_table_t* subtable) {
  myml_value_t* value = myml_get_value(table, key);
  if (!value) {
    table->size++;
    table->values = realloc(table->values, table->size * sizeof(myml_value_t));
    value = &table->values[table->size - 1];
    value->key = myml_strdup(key);
  }
  value->type = MVT_TABLE;
  value->table = subtable;

  return value;
}

myml_iter_t myml_iter(myml_table_t* table) {
  myml_iter_t it = {.cur = table ? table->values : NULL,
                    .end = table ? table->values + table->size : NULL};
  return it;
}

bool myml_end(myml_iter_t it) { return it.cur != it.end; }

void myml_next(myml_iter_t* it) { it->cur++; }

const char* myml_it_key(myml_iter_t it) { return it.cur ? it.cur->key : NULL; }

const char* myml_it_string(myml_iter_t it) {
  return it.cur && it.cur->type == MVT_STRING ? it.cur->string : NULL;
}

myml_table_t* myml_it_subtable(myml_iter_t it) {
  return it.cur && it.cur->type == MVT_TABLE ? it.cur->table : NULL;
}

myml_parse_result_t myml_parse(const char* source) {
  myml_table_t* root_table = myml_alloc();

  myml_parse_context_t ctx;
  memset(&ctx, 0, sizeof(myml_parse_context_t));
  myml_stack_push(&ctx.levels, root_table);

  int column = 1, line = 1, line_start = -1;
  int level = 0, spaces = 0;
  int key_start = -1, key_stop = -1;
  int value_start = -1, value_stop = -1;
  int comment_start = -1;

  for (int i = 0; source[i] != '\0'; i++) {
    char curc = source[i];
    column++;
    if (curc == '\n') {
      ctx.line = line;
      ctx.column = column - 1;

      if (ctx.levels.size <= level) { return myml_error(&ctx, "bad indent"); }

      while (ctx.levels.size > level + 1) myml_stack_pop(&ctx.levels);

      line++;
      column = 1;

      if (key_start != -1) {
        if (key_stop == -1) { return myml_error(&ctx, "expected colon"); }
        ctx.column = key_start - line_start - 1;

        if (value_start != -1 && comment_start != -1) {
          value_stop = comment_start - 1;
        }

        if (value_start != -1 && value_stop == -1) { value_stop = i; }

        char* key = myml_strndup(&source[key_start], key_stop - key_start);
        char* value = NULL;
        if (value_start != -1 && value_stop != -1) {
          value = myml_sanitize_value(&source[value_start],
                                      value_stop - value_start);
        }

        myml_table_t* current_table = myml_stack_top(&ctx.levels);
        assert(current_table);

        if (myml_get_value(current_table, key)) {
          return myml_error(&ctx, "duplicate key");
        }

        myml_value_t* v;
        if (!value) {
          myml_table_t* subtable = myml_alloc();
          v = myml_set_subtable(current_table, key, subtable);
          myml_stack_push(&ctx.levels, subtable);
        } else {
          v = myml_set_string_nocopy(current_table, key, value);
        }
        v->line = ctx.line;
        v->column = ctx.column;
      }

      line_start = i;
      level = 0;
      spaces = 0;
      key_start = -1;
      key_stop = -1;
      value_start = -1;
      value_stop = -1;
      comment_start = -1;
    } else if (curc == '\r') {
      line_start++;
    } else if (curc == ':') {
      if (key_start != -1) { key_stop = i; }
    } else if (curc <= ' ' && key_start == -1) {
      if (curc != ' ') {
        ctx.line = line;
        ctx.column = i - line_start;
        return myml_error(&ctx, "tabs not allowed");
      }
      spaces++;
      if (spaces >= 2) {
        spaces = 0;
        level++;
      }
      if (key_stop != -1 && value_start != -1) { value_stop = i; }
    } else if (curc == '#') {
      comment_start = i;
    } else if (comment_start == -1) {
      if (key_start == -1) {
        key_start = i;
      } else if (key_stop != -1 && value_start == -1) {
        value_start = i;
      }
    }
  }

  myml_parse_result_t result = {.table = root_table, .error = no_error};

  return result;
}

void myml_merge(myml_table_t* dst, myml_table_t* src) {
  if (!dst || !src) return;

  for (int i = 0; i < src->size; i++) {
    myml_value_t* src_value = &src->values[i];
    myml_value_t* dst_value = myml_get_value(dst, src_value->key);
    if (dst_value) {
      dst_value->type = src_value->type;
      dst_value->key = myml_strdup(src_value->key);
      if (src_value->type == MVT_STRING) {
        dst_value->string = myml_strdup(src_value->string);
        dst_value->line = src_value->line;
        dst_value->column = src_value->column;
      } else {
        if (!dst_value->table) { dst_value->table = myml_alloc(); }
        myml_merge(dst_value->table, src_value->table);
      }
      continue;
    }
    if (src_value->type == MVT_STRING) {
      dst_value = myml_set_string(dst, src_value->key, src_value->string,
                                  strlen(src_value->string));
    } else {
      dst_value = myml_set_subtable(dst, src_value->key, myml_alloc());
      myml_merge(dst_value->table, src_value->table);
    }
    dst_value->line = src_value->line;
    dst_value->column = src_value->column;
  }
}

static void myml_remove_at(myml_table_t* table, int index) {
  if (table->size == 0) return;

  myml_value_t* value = &table->values[index];

  free((void*) (uintptr_t) value->key);
  if (value->type == MVT_STRING) {
    free((void*) (uintptr_t) value->string);
  } else {
    myml_free(value->table);
  }

  table->values[index] = table->values[table->size - 1];
  table->size--;
}

#define MYML_INHERITS "inherits"
#define MYML_INHERITS_SIZE sizeof("inherits") - 1

static void myml_merge_base(myml_table_t* table, myml_table_t* base) {
  if (!table || !base) return;

  for (int i = 0; i < base->size; i++) {
    myml_value_t* base_value = &base->values[i];
    myml_value_t* value = myml_get_value(table, base_value->key);
    if (base_value->type == MVT_STRING) {
      if (!value &&
          strncmp(base_value->key, MYML_INHERITS, MYML_INHERITS_SIZE) != 0) {

        myml_set_string(table, base_value->key, base_value->string,
                        strlen(base_value->string));
      }
    } else {
      if (!value) {
        value = myml_set_subtable(table, base_value->key, myml_alloc());
      }
      if (value->type == MVT_TABLE) {
        myml_merge_base(value->table, base_value->table);
      }
    }
  }
}

static int myml_compare_inherits_keys(const void* a, const void* b) {
  myml_value_t* v1 = (myml_value_t*) a;
  myml_value_t* v2 = (myml_value_t*) b;

  int cmp = strcmp(v1->key, v2->key);
  return cmp ? -cmp : 0;
}

static myml_error_t myml_take_inherit_from(myml_table_t* table,
                                           myml_table_t* ref_table) {
  int* to_remove = malloc(table->size * sizeof(int));
  int to_remove_count = 0;

  myml_value_t* base_keys = malloc(table->size * sizeof(myml_value_t));
  int base_keys_count = 0;

  for (int i = 0; i < table->size; i++) {
    if (table->values[i].type != MVT_STRING) continue;
    if (strncmp(table->values[i].key, MYML_INHERITS, MYML_INHERITS_SIZE) == 0) {
      myml_value_t value;
      value.key = table->values[i].key;
      value.string = table->values[i].string;
      base_keys[base_keys_count++] = value;
      to_remove[to_remove_count++] = i;
    }
  }

  qsort(base_keys, base_keys_count, sizeof(myml_value_t),
        myml_compare_inherits_keys);

  for (int i = 0; i < base_keys_count; i++) {
    myml_table_t* base = myml_get_subtable(ref_table, base_keys[i].string);
    if (!base) {
      myml_error_t error = {.line = base_keys[i].line,
                            .column = base_keys[i].column,
                            .message = "base not found"};

      free(base_keys);
      free(to_remove);

      return error;
    }

    myml_merge_base(table, base);
  }

  for (int i = 0; i < to_remove_count; i++) { myml_remove_at(table, i); }

  free(base_keys);
  free(to_remove);

  return no_error;
}

myml_error_t myml_take_inherit(myml_table_t* table) {
  if (!table) return no_error;

  for (int i = 0; i < table->size; i++) {
    myml_value_t* value = &table->values[i];
    if (value->type == MVT_TABLE) {
      myml_error_t error = myml_take_inherit_from(value->table, table);
      if (error.message) return error;
    }
  }

  return no_error;
}
