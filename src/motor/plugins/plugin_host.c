#include "private_plugin.h"

static struct { const char* bin_dir } phost;

void phost_init(const char* bin_dir) {
    log_info("init plugin host");
    log_info("bin_dir: %s", bin_dir);

    phost.bin_dir = mi_strdup(bin_dir);
}

void phost_shutdown() {
    mi_free(phost.bin_dir);
    log_info("shutdown plugin host");
}

plugin_t* phost_open_plugin(const char* name) {
    plugin_t plugin = mi_zalloc(sizeof(plugin_t));
    plugin->context = mi_zalloc(sizeof(plugin_context_t));

    plugin_context_t* context = (plugin_context_t*) plugin->context;
    context->path = mi_strdup(name); // TODO: make fullpath
    context->next_version = 1;
}

void phost_close_plugin(plugin_t* plugin) {
    plugin_context_t* context = (plugin_context_t*) plugin->context;

    // TODO: free context->data

    mi_free(context->path);
    mi_free(context);
    mi_free(plugin);
}

bool phost_load_plugin(plugin_t* plugin) {}

int phost_update_plugin(plugin_t* plugin) {}
