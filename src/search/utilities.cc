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
#include <cstring>

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

static void out_of_memory_handler();
static void signal_handler(int signal_number);


void write_log_message(const char *message) {
    int log_file = open("signal_handler.log", O_WRONLY | O_APPEND | O_CREAT, S_IWRITE | S_IREAD);
    write(log_file, message, strlen(message));
    close(log_file);
}

void register_event_handlers() {
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
    write_log_message("reg\n");
}

#if OPERATING_SYSTEM == LINUX || OPERATING_SYSTEM == OSX
#if OPERATING_SYSTEM == LINUX
void exit_handler(int, void *) {
#elif OPERATING_SYSTEM == OSX
void exit_handler() {
#endif
    write_log_message("in exit_handler\n");
    print_peak_memory(false);
    write_log_message("out exit_handler\n");
}
#endif

void exit_with(ExitCode exitcode) {
    write_log_message("in exit_with\n");
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
    write_log_message("out exit_with\n");
    exit(exitcode);
}

static void out_of_memory_handler() {
    write_log_message("in out_of_memory_handler\n");
    assert(memory_padding);
    delete[] memory_padding;
    memory_padding = 0;
    cout << "Failed to allocate memory. Released memory buffer." << endl;
    write_log_message("out out_of_memory_handler\n");
    exit_with(EXIT_OUT_OF_MEMORY);
}

void signal_handler(int signal_number) {
    write_log_message("in signal_handler\n");
    // See glibc manual: "Handlers That Terminate the Process"
    static volatile sig_atomic_t handler_in_progress = 0;
    write_log_message("signal_handler 1\n");
    if (handler_in_progress) {
        write_log_message("signal_handler if\n");
        raise(signal_number);
    }
    write_log_message("signal_handler 2\n");
    handler_in_progress = 1;
    write_log_message("signal_handler 3\n");
    print_peak_memory(false);
    write_log_message("signal_handler 4\n");
    cout << "caught signal " << signal_number << " -- exiting" << endl;
    write_log_message("signal_handler 5\n");
    signal(signal_number, SIG_DFL);
    write_log_message("out signal_handler\n");
    raise(signal_number);
}

int get_peak_memory_in_kb(bool use_buffered_input) {
    if (!use_buffered_input) write_log_message("in get_peak_memory_in_kb\n");
    // On error, produces a warning on cerr and returns -1.
    int memory_in_kb = -1;
    if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 1\n");

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
    if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 2\n");
    if (!use_buffered_input) {
        write_log_message("get_peak_memory_in_kb 3\n");
        procfile.rdbuf()->pubsetbuf(0, 0);
    }
    if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 4\n");
    procfile.open("/proc/self/status");
    if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 5\n");
    string word;
    if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 6\n");
    while (procfile.good()) {
        if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 7a\n");
        procfile >> word;
        if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 7b\n");
        if (word == "VmPeak:") {
            if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 7c\n");
            procfile >> memory_in_kb;
            if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 7d\n");
            break;
        }
        // Skip to end of line.
        procfile.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 8\n");
    if (procfile.fail()) {
        if (!use_buffered_input) write_log_message("get_peak_memory_in_kb failed\n");
        memory_in_kb = -1;
    }
#endif

    if (!use_buffered_input) write_log_message("get_peak_memory_in_kb 9\n");
    if (memory_in_kb == -1) {
        if (!use_buffered_input) write_log_message("get_peak_memory_in_kb failed 2\n");
        cerr << "warning: could not determine peak memory" << endl;
    }
    if (!use_buffered_input) write_log_message("out get_peak_memory_in_kb\n");
    return memory_in_kb;
}

void print_peak_memory(bool use_buffered_input) {
    if (!use_buffered_input) write_log_message("in print_peak_memory\n");
    cout << "Peak memory: " << get_peak_memory_in_kb(use_buffered_input) << " KB" << endl;
    if (!use_buffered_input) write_log_message("out print_peak_memory\n");
}


bool is_product_within_limit(int factor1, int factor2, int limit) {
    assert(factor1 >= 0 && factor1 <= limit);
    assert(factor2 >= 0 && factor2 <= limit);
    return factor2 == 0 || factor1 <= limit / factor2;
}
