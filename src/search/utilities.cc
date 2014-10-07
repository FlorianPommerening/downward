#include "utilities.h"

#include <cassert>
#include <csignal>
#include <iostream>
#include <fstream>
#include <limits>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;


#if OPERATING_SYSTEM == LINUX
static void exit_handler(int exit_code, void *hint);
#elif OPERATING_SYSTEM == OSX
static void exit_handler();
#include <mach/mach.h>
#elif OPERATING_SYSTEM == WINDOWS || OPERATING_SYSTEM == CYGWIN
#include <windows.h>
#include <psapi.h>
#endif

// See issue 469 for the reasons we chose this limit.
static char *memory_padding = new char[32 * 1024];

static int signal_handler_log;

static void out_of_memory_handler();
static void signal_handler(int signal_number);


void register_event_handlers() {
    signal_handler_log = open("signal_handler.log", O_WRONLY | O_CREAT, S_IWRITE | S_IREAD);
    // When running out of memory, release some emergency memory and
    // terminate.
    set_new_handler(out_of_memory_handler);

    // On exit or when receiving certain signals such as SIGINT (Ctrl-C),
    // print the peak memory usage.
#if OPERATING_SYSTEM == LINUX
    on_exit(exit_handler, 0);
#elif OPERATING_SYSTEM == OSX
    atexit(exit_handler);
#elif OPERATING_SYSTEM == CYGWIN || OPERATING_SYSTEM == WINDOWS
    // nothing
#endif
    signal(SIGABRT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGXCPU, signal_handler);
    write(signal_handler_log, "reg\n", 4);
}

#if OPERATING_SYSTEM == LINUX || OPERATING_SYSTEM == OSX
#if OPERATING_SYSTEM == LINUX
void exit_handler(int, void *) {
#elif OPERATING_SYSTEM == OSX
void exit_handler() {
#endif
    write(signal_handler_log, "in exit_handler\n", 16);
    print_peak_memory(false);
    write(signal_handler_log, "out exit_handler\n", 17);
    close(signal_handler_log);
}
#endif

void exit_with(ExitCode exitcode) {
    write(signal_handler_log, "in exit_with\n", 13);
    switch (exitcode) {
    case EXIT_PLAN_FOUND:
        cout << "Solution found." << endl;
        break;
    case EXIT_CRITICAL_ERROR:
        cerr << "Unexplained error occurred." << endl;
        break;
    case EXIT_INPUT_ERROR:
        cerr << "Usage error occurred." << endl;
        break;
    case EXIT_UNSUPPORTED:
        cerr << "Tried to use unsupported feature." << endl;
        break;
    case EXIT_UNSOLVABLE:
        cout << "Task is provably unsolvable." << endl;
        break;
    case EXIT_UNSOLVED_INCOMPLETE:
        cout << "Search stopped without finding a solution." << endl;
        break;
    case EXIT_OUT_OF_MEMORY:
        cout << "Memory limit has been reached." << endl;
        break;
    case EXIT_TIMEOUT:
        cout << "Time limit has been reached." << endl;
        break;
    default:
        cerr << "Exitcode: " << exitcode << endl;
        ABORT("Unknown exitcode.");
    }
    write(signal_handler_log, "out exit_with\n", 14);
    exit(exitcode);
}

static void out_of_memory_handler() {
    write(signal_handler_log, "in out_of_memory_handler\n", 25);
    assert(memory_padding);
    delete[] memory_padding;
    memory_padding = 0;
    cout << "Failed to allocate memory. Released memory buffer." << endl;
    write(signal_handler_log, "out out_of_memory_handler\n", 26);
    exit_with(EXIT_OUT_OF_MEMORY);
}

void signal_handler(int signal_number) {
    write(signal_handler_log, "in signal_handler\n", 18);
    // See glibc manual: "Handlers That Terminate the Process"
    static volatile sig_atomic_t handler_in_progress = 0;
    write(signal_handler_log, "signal_handler 1\n", 17);
    if (handler_in_progress) {
        write(signal_handler_log, "signal_handler if\n", 18);
        raise(signal_number);
    }
    write(signal_handler_log, "signal_handler 2\n", 17);
    handler_in_progress = 1;
    write(signal_handler_log, "signal_handler 3\n", 17);
    print_peak_memory(false);
    write(signal_handler_log, "signal_handler 4\n", 17);
    cout << "caught signal " << signal_number << " -- exiting" << endl;
    write(signal_handler_log, "signal_handler 5\n", 17);
    signal(signal_number, SIG_DFL);
    write(signal_handler_log, "out signal_handler\n", 19);
    close(signal_handler_log);
    raise(signal_number);
}

int get_peak_memory_in_kb(bool use_buffered_input) {
    if (!use_buffered_input) write(signal_handler_log, "in get_peak_memory_in_kb\n", 25);
    // On error, produces a warning on cerr and returns -1.
    int memory_in_kb = -1;
    if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 1\n", 24);

#if OPERATING_SYSTEM == OSX
    // Based on http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), TASK_BASIC_INFO,
                  reinterpret_cast<task_info_t>(&t_info),
                  &t_info_count) == KERN_SUCCESS)
        memory_in_kb = t_info.virtual_size / 1024;
#elif OPERATING_SYSTEM == WINDOWS || OPERATING_SYSTEM == CYGWIN
    // The file /proc/self/status is present under Cygwin, but contains no peak memory info.
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc), sizeof(pmc));
    memory_in_kb = pmc.PeakPagefileUsage / 1024;
#else
    ifstream procfile;
    if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 2\n", 24);
    if (!use_buffered_input) {
        write(signal_handler_log, "get_peak_memory_in_kb 3\n", 24);
        procfile.rdbuf()->pubsetbuf(0, 0);
    }
    if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 4\n", 24);
    procfile.open("/proc/self/status");
    if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 5\n", 24);
    string word;
    if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 6\n", 24);
    while (procfile.good()) {
        if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 7a\n", 25);
        procfile >> word;
        if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 7b\n", 25);
        if (word == "VmPeak:") {
            if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 7c\n", 25);
            procfile >> memory_in_kb;
            if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 7d\n", 25);
            break;
        }
        // Skip to end of line.
        procfile.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 8\n", 24);
    if (procfile.fail()) {
        if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb failed\n", 29);
        memory_in_kb = -1;
    }
#endif

    if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb 9\n", 24);
    if (memory_in_kb == -1) {
        if (!use_buffered_input) write(signal_handler_log, "get_peak_memory_in_kb failed 2\n", 31);
        cerr << "warning: could not determine peak memory" << endl;
    }
    if (!use_buffered_input) write(signal_handler_log, "out get_peak_memory_in_kb\n", 26);
    return memory_in_kb;
}

void print_peak_memory(bool use_buffered_input) {
    if (!use_buffered_input) write(signal_handler_log, "in print_peak_memory\n", 21);
    cout << "Peak memory: " << get_peak_memory_in_kb(use_buffered_input) << " KB" << endl;
    if (!use_buffered_input) write(signal_handler_log, "out print_peak_memory\n", 22);
}


bool is_product_within_limit(int factor1, int factor2, int limit) {
    assert(factor1 >= 0 && factor1 <= limit);
    assert(factor2 >= 0 && factor2 <= limit);
    return factor2 == 0 || factor1 <= limit / factor2;
}
