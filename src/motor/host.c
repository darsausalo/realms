#include "motor.h"

#include "private_api.h"

const char* plugin = FRONTIER_BINARY_DIR "/" CR_PLUGIN(FRONTIER_GAME_PLUGIN);

static const char* Host_GetText(int i) { return "Test"; }

void printmemstat(const char* msg, void* arg) { printf("%s", msg); }


int main(int argc, char* argv[]) {
  printf("initalize: mi version: %d\n", mi_version());

  sys_register_crash_handler(FRONTIER_BINARY_DIR);

  hostApi_t api;

  api.GetText = Host_GetText;

  myml_parse_result_t result = myml_parse(Host_GetText(0));

  cr_plugin ctx;
  ctx.userdata = &api;
  // the host application should initalize a plugin with a context, a plugin
  // filename without extension and the full path to the plugin
  cr_plugin_open(&ctx, plugin);

  mi_stats_print_out(printmemstat, NULL);

  // call the plugin update function with the plugin context to execute it
  // at any frequency matters to you
  while (true) {
    cr_plugin_update(&ctx, true);
    fflush(stdout);
    fflush(stderr);
  }

  // at the end do not forget to cleanup the plugin context, as it needs to
  // allocate some memory to track internal and plugin states
  cr_plugin_close(&ctx);

  return 0;
}
