#include "private_api.h"
#include <mimalloc.h>

bool want_quit = false;

static void interrupt_handler() { want_quit = true; }

typedef struct {
    int   foo;
    float bar;
} my_st_t;

int main(int argc, char* argv[]) {
    host_init(argc, argv);

    while (host_is_run()) {}

    host_shutdown();

    return 0;
}
