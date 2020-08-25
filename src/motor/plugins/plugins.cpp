extern "C" {
#include <stdio.h>

#include "motor/log.h"
#include "motor/plugins/plugin.h"

#include "private_api.h"
}

#include <cstdio>

#include "cr.h"

static bool cr_plugin_load_internal(cr_plugin& ctx, bool rollback) {
  CR_TRACE
  auto p = (cr_internal*) ctx.p;
  const std::string file = p->fullname;
  if (cr_exists(file) || rollback) {
    const auto old_file = cr_version_path(file, ctx.version);
    CR_LOG("unload '%s' with rollback: %d\n", old_file.c_str(), rollback);
    int r = cr_plugin_unload(ctx, rollback, false);
    if (r < 0) { return false; }

    auto new_version = rollback ? ctx.version : ctx.next_version;
    auto new_file = cr_version_path(file, new_version);
    if (rollback) {
      if (ctx.version == 0) {
        ctx.failure = CR_INITIAL_FAILURE;
        return false;
      }
      // Don't rollback to this version again, if it crashes.
      ctx.last_working_version = ctx.version > 0 ? ctx.version - 1 : 0;
    } else {
      // Save current version for rollback.
      ctx.last_working_version = ctx.version;
      cr_copy(file, new_file);

      // Update `next_version` for use by the next reload.
      ctx.next_version = new_version + 1;

#if defined(_MSC_VER)
      if (!cr_pdb_process(new_file)) {
        CR_ERROR("Couldn't process PDB, debugging may be "
                 "affected and/or reload may fail\n");
      }
#endif // defined(_MSC_VER)
    }

    auto new_dll = cr_so_load(new_file);
    if (!new_dll) {
      ctx.failure = CR_BAD_IMAGE;
      return false;
    }

    if (!cr_plugin_validate_sections(ctx, new_dll, new_file, rollback)) {
      return false;
    }

    if (rollback) {
      cr_plugin_sections_reload(ctx, cr_plugin_section_version::backup);
    } else if (ctx.version) {
      cr_plugin_sections_reload(ctx, cr_plugin_section_version::current);
    }

    auto new_main = cr_so_symbol(new_dll);
    if (!new_main) { return false; }

    auto p2 = (cr_internal*) ctx.p;
    p2->handle = new_dll;
    p2->main = new_main;
    if (ctx.failure != CR_BAD_IMAGE) {
      p2->timestamp = cr_last_write_time(file);
    }
    ctx.version = new_version;
    CR_LOG("loaded: %s (version: %d)\n", new_file.c_str(), ctx.version);
  } else {
    CR_ERROR("Error loading plugin.\n");
    return false;
  }
  return true;
}

static bool cr_plugin_section_validate(cr_plugin& ctx,
                                       cr_plugin_section_type::e type,
                                       intptr_t ptr, intptr_t base,
                                       int64_t size) {
  CR_TRACE(void) ptr;
  auto p = (cr_internal*) ctx.p;
  return (p->data[type][0].base == base && p->data[type][0].size == size);
}

// internal
static void cr_plugin_sections_backup(cr_plugin& ctx) {
  auto p = (cr_internal*) ctx.p;
  CR_TRACE

  for (int i = 0; i < cr_plugin_section_type::count; ++i) {
    auto cur = &p->data[i][cr_plugin_section_version::current];
    if (cur->ptr) {
      auto bkp = &p->data[i][cr_plugin_section_version::backup];
      bkp->data = CR_REALLOC(bkp->data, cur->size);
      bkp->ptr = cur->ptr;
      bkp->size = cur->size;
      bkp->base = cur->base;

      if (bkp->data) { std::memcpy(bkp->data, cur->data, bkp->size); }
    }
  }
}

// internal
// Before unloading iterate over possible global static state and keeps an
// internal copy to be used in next version load and a backup copy as a known
// valid state checkpoint. This is mostly due that a new load may want to
// modify the state and if anything bad happens we are sure to have a valid
// and compatible copy of the state for the previous version of the plugin.
static void cr_plugin_sections_store(cr_plugin& ctx) {
  auto p = (cr_internal*) ctx.p;
  CR_TRACE

  auto version = cr_plugin_section_version::current;
  for (int i = 0; i < cr_plugin_section_type::count; ++i) {
    if (p->data[i][version].ptr && p->data[i][version].data) {
      const char* ptr = p->data[i][version].ptr;
      const int64_t len = p->data[i][version].size;
      std::memcpy(p->data[i][version].data, ptr, len);
    }
  }

  cr_plugin_sections_backup(ctx);
}

// internal
// After a load happens reload the global state from previous version from our
// internal copy created during the unload step.
static void cr_plugin_sections_reload(cr_plugin& ctx,
                                      cr_plugin_section_version::e version) {
  CR_ASSERT(version < cr_plugin_section_version::count);
  auto p = (cr_internal*) ctx.p;
  CR_TRACE

  for (int i = 0; i < cr_plugin_section_type::count; ++i) {
    if (p->data[i][version].data) {
      const int64_t len = p->data[i][version].size;
      // restore backup into the current section address as it may
      // change due aslr and backup address may be invalid
      const auto current = cr_plugin_section_version::current;
      auto dest = (void*) p->data[i][current].ptr;
      if (dest) { std::memcpy(dest, p->data[i][version].data, len); }
    }
  }
}

// internal
// Cleanup and frees any temporary memory used to keep global static data
// between sessions, used during shutdown.
static void cr_so_sections_free(cr_plugin& ctx) {
  CR_TRACE
  auto p = (cr_internal*) ctx.p;
  for (int i = 0; i < cr_plugin_section_type::count; ++i) {
    for (int v = 0; v < cr_plugin_section_version::count; ++v) {
      if (p->data[i][v].data) { CR_FREE(p->data[i][v].data); }
      p->data[i][v].data = nullptr;
    }
  }
}

static bool cr_plugin_changed(cr_plugin& ctx) {
  auto p = (cr_internal*) ctx.p;
  const auto src = cr_last_write_time(p->fullname);
  const auto cur = p->timestamp;
  return src > cur;
}

// internal
// Unload current running plugin, if it is not a rollback it will trigger a
// last update with `cr_op::CR_UNLOAD` (that may crash and cause another
// rollback, etc.) storing global static states to use with next load. If the
// unload is due a rollback, no `cr_op::CR_UNLOAD` is called neither any state
// is saved, giving opportunity to the previous version to continue with valid
// previous state.
static int cr_plugin_unload(cr_plugin& ctx, bool rollback, bool close) {
  CR_TRACE
  auto p = (cr_internal*) ctx.p;
  int r = 0;
  if (p->handle) {
    if (!rollback) {
      r = cr_plugin_main(ctx, close ? CR_CLOSE : CR_UNLOAD);
      // Don't store state if unload crashed.  Rollback will use backup.
      if (r < 0) {
        CR_LOG("4 FAILURE: %d\n", r);
      } else {
        cr_plugin_sections_store(ctx);
      }
    }
    cr_so_unload(ctx);
    p->handle = nullptr;
    p->main = nullptr;
  }
  return r;
}

// internal
// Force a version rollback, causing a partial-unload and a load with the
// previous version, also triggering an update with `cr_op::CR_LOAD` that
// in turn may also cause more rollbacks.
static bool cr_plugin_rollback(cr_plugin& ctx) {
  CR_TRACE
  auto loaded = cr_plugin_load_internal(ctx, true);
  if (loaded) {
    loaded = cr_plugin_main(ctx, CR_LOAD) >= 0;
    if (loaded) { ctx.failure = CR_NONE; }
  }
  return loaded;
}

// internal
// Checks if a rollback or a reload is needed, do the unload/loading and call
// update one time with `cr_op::CR_LOAD`. Note that this may fail due to crash
// handling during this first update, effectivelly rollbacking if possible and
// causing a consecutive `CR_LOAD` with the previous version.
extern "C" void cr_plugin_reload(cr_plugin& ctx) {
  if (cr_plugin_changed(ctx)) {
    CR_TRACE
    if (!cr_plugin_load_internal(ctx, false)) { return; }
    int r = cr_plugin_main(ctx, CR_LOAD);
    if (r < 0 && !ctx.failure) {
      CR_LOG("2 FAILURE: %d\n", r);
      ctx.failure = CR_USER;
    }
  }
}

// This is basically the plugin `main` function, should be called as
// frequently as your core logic/application needs. -1 and -2 are the only
// possible return values from cr meaning a fatal error (causes rollback),
// other return values are returned directly from `cr_main`.
extern "C" int cr_plugin_update(cr_plugin* ctx, bool reloadCheck) {
  if (ctx->failure) {
    CR_LOG("1 ROLLBACK version was %d\n", ctx->version);
    cr_plugin_rollback(*ctx);
    CR_LOG("1 ROLLBACK version is now %d\n", ctx->version);
  } else {
    if (reloadCheck) { cr_plugin_reload(*ctx); }
  }

  // -2 to differentiate from crash handling code path, meaning the crash
  // happened probably during load or unload and not update
  if (ctx->failure) {
    CR_LOG("3 FAILURE: -2\n");
    return -2;
  }

  int r = cr_plugin_main(*ctx, CR_STEP);
  if (r < 0 && !ctx->failure) {
    CR_LOG("4 FAILURE: CR_USER\n");
    ctx->failure = CR_USER;
  }
  return r;
}

// Loads a plugin from the specified full path (or current directory if NULL).
extern "C" bool cr_plugin_open(cr_plugin* ctx, const char* fullpath) {
  CR_TRACE
  CR_ASSERT(fullpath);
  if (!cr_exists(fullpath)) { return false; }
  auto p = new (CR_MALLOC(sizeof(cr_internal))) cr_internal;
  p->fullname = fullpath;
  ctx->p = p;
  ctx->next_version = 1;
  ctx->last_working_version = 0;
  ctx->version = 0;
  ctx->failure = CR_NONE;
  cr_plat_init();
  log_debug("ctx->version: %d", ctx->version);
  return true;
}

// Call to cleanup internal state once the plugin is not required anymore.
extern "C" void cr_plugin_close(cr_plugin* ctx) {
  CR_TRACE
  const bool rollback = false;
  const bool close = true;
  cr_plugin_unload(*ctx, rollback, close);
  cr_so_sections_free(*ctx);
  auto p = (cr_internal*) ctx->p;

  // delete backups
  const auto file = p->fullname;
  for (unsigned int i = 0; i < ctx->version; i++) {
    cr_del(cr_version_path(file, i));
#if defined(_MSC_VER)
    cr_del(cr_replace_extension(cr_version_path(file, i), ".pdb"));
#endif
  }

  p->~cr_internal();
  CR_FREE(p);
  ctx->p = nullptr;
  ctx->version = 0;
}
