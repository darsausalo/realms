#include "private_api.h"
#include <mimalloc.h>
#include <locale.h>
#include <uchar.h>

static mt_atomic_int_t host_want_quit;
static FILE*           host_log_file;

static void host_check_memory_leaks() {
    log_info("   malloc count: %d", mt_malloc_count);
    log_info("  realloc count: %d", mt_realloc_count);
    log_info("   calloc count: %d", mt_calloc_count);
    log_info("     free count: %d", mt_free_count);

    int alloc_count = (mt_malloc_count + mt_calloc_count);
    if (mt_free_count != alloc_count) { log_error("memory leak detected!"); }
}

static myml_table_t* host_parse_cli_options(int argc, char** argv) {
    myml_table_t* options = myml_alloc();

    const char* key = NULL;
    for (int i = 0; i < argc; i++) {
        int len = strlen(argv[i]);
        if (len < 3 || argv[i][0] != '-' || argv[i][1] != '-') {
            if (key) {
                myml_set_path_string(options, key, argv[i]);
                key = NULL;
            }
            continue;
        }
        if (key) { log_warning("argument without value: %s", key); }
        key = &argv[i][2];
    }
    if (key) { log_warning("argument without value: %s", key); }

    return options;
}

void host_init(int argc, char** argv) {
    sys_init();

    host_log_file = sys_open_log_file();
    log_set_fp(host_log_file, kLogTrace);
    log_info("init host: start (тест, 世界)");
    log_info("     version: %s", MOTOR_PROJECT_VERSION);
    log_info("  mi version: %d", mi_version());
    log_info("          os: %s", MOTOR_OS);

    host_want_quit = 0;

    myml_table_t* cli_options = host_parse_cli_options(argc, argv);

    fs_init(cli_options);

    myml_free(cli_options);

    sys_set_interrupt_handler(host_abort);

    log_info("init host: done");
}

void host_shutdown() {
    log_info("shutdown host: start");

    fs_shutdown();

    host_check_memory_leaks();

    log_info("shutdown host: done");

    fflush(host_log_file);
    fclose(host_log_file);

    sys_shutdown();
}

bool host_is_run() { return !host_want_quit; }

void host_quit() {
    log_debug("request quit");
    host_want_quit = mt_axchg(&host_want_quit, 1);
}

void host_abort() {
    host_shutdown();
    exit(1);
}
