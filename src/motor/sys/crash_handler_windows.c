#include "sys_windows.h"

#pragma warning(push)
#pragma warning(disable : 4091)
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#pragma warning(pop)

#define MAX_STACK_FRAMES 16

typedef struct stack_frame_t {
    size_t      num;
    uintptr_t   addr;
    const char* filename;
    const char* modname;
    const char* funcname;
    size_t      line;
} stack_frame_t;

static const char* sys_sym_search_path = NULL;
static void (*sys_interrupt_callback)() = NULL;

static void sys_print_frame(const stack_frame_t* frame) {
    fprintf(stderr, "#%zu\tSource \"%s\", line %zu in %s [0x%zx]\n", frame->num,
            frame->filename, frame->line, frame->funcname, frame->addr);
}

void sys_dump_stack_trace(void* context) {
    STACKFRAME64 stack_frame;
    memset(&stack_frame, 0, sizeof(stack_frame));

    LPEXCEPTION_POINTERS ep = (LPEXCEPTION_POINTERS) context;

    DWORD machineType;

#if defined(_M_X64)
    machineType = IMAGE_FILE_MACHINE_AMD64;
    stack_frame.AddrPC.Offset = ep->ContextRecord->Rip;
    stack_frame.AddrPC.Mode = AddrModeFlat;
    stack_frame.AddrStack.Offset = ep->ContextRecord->Rsp;
    stack_frame.AddrStack.Mode = AddrModeFlat;
    stack_frame.AddrFrame.Offset = ep->ContextRecord->Rbp;
    stack_frame.AddrFrame.Mode = AddrModeFlat;
#elif defined(_M_IX86)
    machineType = IMAGE_FILE_MACHINE_I386;
    stack_frame.AddrPC.Offset = ep->ContextRecord->Eip;
    stack_frame.AddrPC.Mode = AddrModeFlat;
    stack_frame.AddrStack.Offset = ep->ContextRecord->Esp;
    stack_frame.AddrStack.Mode = AddrModeFlat;
    stack_frame.AddrFrame.Offset = ep->ContextRecord->Ebp;
    stack_frame.AddrFrame.Mode = AddrModeFlat;
#endif

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    SymInitialize(hProcess, sys_sym_search_path, TRUE);
    SymRefreshModuleList(hProcess);

    fprintf(stderr, "\033[31;1m*** FATAL ERROR ***\n");

    size_t frame_num = 0;
    while (true) {
        if (frame_num >= MAX_STACK_FRAMES) { break; }

        if (!StackWalk64(machineType, hProcess, hThread, &stack_frame,
                         ep->ContextRecord, NULL, SymFunctionTableAccess64,
                         SymGetModuleBase64, NULL)) {
            break;
        }
        if (stack_frame.AddrFrame.Offset == 0) { break; }

        frame_num++;

        uintptr_t PC = stack_frame.AddrPC.Offset;

        stack_frame_t frame;
        frame.num = frame_num;
        frame.addr = PC;
        frame.modname = "<unknown>";
        frame.funcname = "<unknown>";
        frame.filename = "<unknown>";
        frame.line = 0;

        if (frame_num == 1) fprintf(stderr, "\033[33m");
        else
            fprintf(stderr, "\033[37;0m");

        DWORD64 modbase = SymGetModuleBase64(hProcess, PC);
        if (!modbase) {
            sys_print_frame(&frame);
            continue;
        }

        char modname[MAX_PATH];
        if (GetModuleFileNameA((HINSTANCE)(modbase), modname, MAX_PATH)) {
            frame.modname = modname;
        }

        char               buffer[512];
        IMAGEHLP_SYMBOL64* symbol = (IMAGEHLP_SYMBOL64*) (buffer);
        memset(buffer, 0, sizeof(IMAGEHLP_SYMBOL64));
        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        symbol->MaxNameLength = 512 - sizeof(IMAGEHLP_SYMBOL64);

        DWORD64 dwDisp = 0;
        if (SymGetSymFromAddr64(hProcess, PC, &dwDisp, symbol)) {

            frame.funcname = symbol->Name;

            IMAGEHLP_LINE64 image_line;
            DWORD           dwLineDisp = 0;
            memset(&image_line, 0, sizeof(image_line));
            image_line.SizeOfStruct = sizeof(image_line);
            if (SymGetLineFromAddr64(hProcess, PC, &dwLineDisp, &image_line)) {
                frame.filename = image_line.FileName;
                frame.line = image_line.LineNumber;
            } else {
                frame.filename = "<unknown>";
                frame.line = 0;
            }
        }

        sys_print_frame(&frame);
    }

    fflush(stdout);
    fflush(stderr);
}

static LONG WINAPI sys_error_handler(LPEXCEPTION_POINTERS ep) {
    sys_dump_stack_trace((void*) ep);

    _exit(ep->ExceptionRecord->ExceptionCode);

    return EXCEPTION_CONTINUE_SEARCH;
}

static BOOL WINAPI sys_interrupt_handler(DWORD dwCtrlType) {
    if (sys_interrupt_callback) {
        sys_interrupt_callback();
        return TRUE;
    }
    return FALSE;
}

void sys_register_crash_handler() {
    SetUnhandledExceptionFilter(sys_error_handler);
    SetConsoleCtrlHandler(sys_interrupt_handler, TRUE);
}

void sys_set_sym_search_path(const char* sym_search_path) {
    sys_sym_search_path = sym_search_path;
}

void sys_set_interrupt_handler(void (*interrupt_handler_func)()) {
    sys_interrupt_callback = interrupt_handler_func;
}
