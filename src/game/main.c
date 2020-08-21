#include "motor.h"

#include "motor/host/cr.h"

CR_EXPORT int cr_main(struct cr_plugin* ctx, enum cr_op operation) {
  assert(ctx);
  switch (operation) {
  case CR_LOAD:
    printf("loaded\n");
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
    print_version = false;
  }
  return 0;
}
