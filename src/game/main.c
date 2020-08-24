#include "motor.h"

void call_crash() {
  int* x = NULL;
  *x = 1;
}

void crash_test() { call_crash(); }

CR_EXPORT int cr_main(struct cr_plugin* ctx, enum cr_op operation) {
  assert(ctx);
  switch (operation) {
  case CR_LOAD:
    printf("loaded[2]\n");
    return 0;
  case CR_UNLOAD:
    printf("unloaded\n");
    return 0;
  case CR_CLOSE:
    printf("closed\n");
    return 0;
  }

  static bool print_version = true;
  if (print_version) {
    printf("loaded version: %d\n", ctx->version);
    // crash_test();
    print_version = false;
  }
  return 0;
}
