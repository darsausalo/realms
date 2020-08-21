#include "motor.h"
#include <chrono>
#include <cstdio>
#include <thread>

#define CR_HOST CR_SAFEST // try to best manage static states
#include "motor/host/cr.h"

const char* plugin = FRONTIER_DEPLOY_DIR "/" CR_PLUGIN(FRONTIER_GAME_PLUGIN);

static const char* src1 = R"(entity_base:
  health:
    max: 100
  speed:
    max: 101
    accel: 11
  tag1:

entity1:
  inherits: entity_base
  health:
    max: 200
  tag2:
  tst1: x1

entity2:
  inherits@1: entity_base
  inherits@2: entity1
  speed:
    max: 300
  tag3:
)";

static const char* src2 = R"(entity_base:
  tag4:

entity1:
  health:
    max: 215

entity2:
  speed: 111
  armor:
    value: 15
)";

static const char* Host_GetText(int i) { return i == 0 ? src1 : src2; }

int main(int argc, char* argv[]) {
  hostApi_t api;

  api.GetText = Host_GetText;

  myml_parse_result_t result = myml_parse(Host_GetText(0));

  cr_plugin ctx;
  ctx.userdata = &api;
  // the host application should initalize a plugin with a context, a plugin
  // filename without extension and the full path to the plugin
  cr_plugin_open(ctx, plugin);

  // call the plugin update function with the plugin context to execute it
  // at any frequency matters to you
  while (true) {
    cr_plugin_update(ctx);
    fflush(stdout);
    fflush(stderr);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // at the end do not forget to cleanup the plugin context, as it needs to
  // allocate some memory to track internal and plugin states
  cr_plugin_close(ctx);
  return 0;
}
