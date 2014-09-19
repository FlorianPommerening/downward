#include "utilities.h"

#include <cassert>
#include <csignal>
#include <iostream>
#include <fstream>
#include <limits>
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

static char *memory_padding = new char[512 * 1024];
FILE * memory_debug_file = fopen("memory_debug.log", "w");

static void out_of_memory_handler();
static void signal_handler(int signal_number);


void register_event_handlers() {
    // When running out of memory, release some emergency memory and
    // terminate.
    set_new_handler(out_of_memory_handler);
    fputs("Handlers set\n", memory_debug_file); fflush(memory_debug_file);

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
}

#if OPERATING_SYSTEM == LINUX || OPERATING_SYSTEM == OSX
#if OPERATING_SYSTEM == LINUX
void exit_handler(int, void *) {
#elif OPERATING_SYSTEM == OSX
void exit_handler() {
#endif
    fputs("Enter exit_handler\n", memory_debug_file); fflush(memory_debug_file);
    print_peak_memory();
    fputs("Exit exit_handler\n", memory_debug_file); fflush(memory_debug_file);
    fclose(memory_debug_file);
}
#endif

void exit_with(ExitCode exitcode) {
    fputs("Enter exit_with\n", memory_debug_file); fflush(memory_debug_file);
    switch (exitcode) {
    case EXIT_PLAN_FOUND:
        fputs("exit_with case 1\n", memory_debug_file); fflush(memory_debug_file);
        cout << "Solution found." << endl;
        break;
    case EXIT_CRITICAL_ERROR:
        fputs("exit_with case 2\n", memory_debug_file); fflush(memory_debug_file);
        cerr << "Unexplained error occurred." << endl;
        break;
    case EXIT_INPUT_ERROR:
        fputs("exit_with case 3\n", memory_debug_file); fflush(memory_debug_file);
        cerr << "Usage error occurred." << endl;
        break;
    case EXIT_UNSUPPORTED:
        fputs("exit_with case 4\n", memory_debug_file); fflush(memory_debug_file);
        cerr << "Tried to use unsupported feature." << endl;
        break;
    case EXIT_UNSOLVABLE:
        fputs("exit_with case 5\n", memory_debug_file); fflush(memory_debug_file);
        cout << "Task is provably unsolvable." << endl;
        break;
    case EXIT_UNSOLVED_INCOMPLETE:
        fputs("exit_with case 6\n", memory_debug_file); fflush(memory_debug_file);
        cout << "Search stopped without finding a solution." << endl;
        break;
    case EXIT_OUT_OF_MEMORY:
        fputs("exit_with case 7\n", memory_debug_file); fflush(memory_debug_file);
        cout << "Memory limit has been reached." << endl;
        break;
    case EXIT_TIMEOUT:
        fputs("exit_with case 8\n", memory_debug_file); fflush(memory_debug_file);
        cout << "Time limit has been reached." << endl;
        break;
    default:
        fputs("exit_with default case\n", memory_debug_file); fflush(memory_debug_file);
        cerr << "Exitcode: " << exitcode << endl;
        ABORT("Unkown exitcode.");
    }
    fputs("Exit exit_with\n", memory_debug_file); fflush(memory_debug_file);
    exit(exitcode);
}

static void out_of_memory_handler() {
    fputs("Enter out_of_memory_handler\n", memory_debug_file); fflush(memory_debug_file);
    assert(memory_padding);
    fputs("out_of_memory_handler after assertion\n", memory_debug_file); fflush(memory_debug_file);
    delete[] memory_padding;
    memory_padding = 0;
    fputs("out_of_memory_handler after delete\n", memory_debug_file); fflush(memory_debug_file);
    cout << "Failed to allocate memory. Released memory buffer." << endl;
    fputs("Exit out_of_memory_handler\n", memory_debug_file); fflush(memory_debug_file);
    exit_with(EXIT_OUT_OF_MEMORY);
}

void signal_handler(int signal_number) {
    fputs("Enter signal_handler\n", memory_debug_file); fflush(memory_debug_file);
    // See glibc manual: "Handlers That Terminate the Process"
    static volatile sig_atomic_t handler_in_progress = 0;
    fputs("signal_handler before if-statement\n", memory_debug_file); fflush(memory_debug_file);
    if (handler_in_progress) {
        fputs("signal_handler within if-statement\n", memory_debug_file); fflush(memory_debug_file);
        raise(signal_number);
    }
    fputs("signal_handler after if-statement\n", memory_debug_file); fflush(memory_debug_file);
    handler_in_progress = 1;
    fputs("signal_handler before print_peak_memory\n", memory_debug_file); fflush(memory_debug_file);
    print_peak_memory();
    fputs("signal_handler after print_peak_memory\n", memory_debug_file); fflush(memory_debug_file);
    cout << "caught signal " << signal_number << " -- exiting" << endl;
    fputs("signal_handler after cout\n", memory_debug_file); fflush(memory_debug_file);
    signal(signal_number, SIG_DFL);
    fputs("Exit signal_handler\n", memory_debug_file); fflush(memory_debug_file);
    fclose(memory_debug_file);
    raise(signal_number);
}

int get_peak_memory_in_kb() {
    fputs("Enter get_peak_memory_in_kb\n", memory_debug_file); fflush(memory_debug_file);
    // On error, produces a warning on cerr and returns -1.
    int memory_in_kb = -1;

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
    GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc));
    memory_in_kb = pmc.PeakPagefileUsage / 1024;
#else
    fputs("get_peak_memory_in_kb create stream\n", memory_debug_file); fflush(memory_debug_file);
    ifstream procfile("/proc/self/status");
    string word;
    fputs("get_peak_memory_in_kb before loop\n", memory_debug_file); fflush(memory_debug_file);
    while (procfile.good()) {
        procfile >> word;
        fputs("get_peak_memory_in_kb inside loop\n", memory_debug_file); fflush(memory_debug_file);
        if (word == "VmPeak:") {
            fputs("get_peak_memory_in_kb found VmPeak\n", memory_debug_file); fflush(memory_debug_file);
            procfile >> memory_in_kb;
            fputs("get_peak_memory_in_kb read peak mem\n", memory_debug_file); fflush(memory_debug_file);
            break;
        }
        fputs("get_peak_memory_in_kb before ignore\n", memory_debug_file); fflush(memory_debug_file);
        // Skip to end of line.
        procfile.ignore(numeric_limits<streamsize>::max(), '\n');
        fputs("get_peak_memory_in_kb after ignore\n", memory_debug_file); fflush(memory_debug_file);
    }
    fputs("get_peak_memory_in_kb check for fail\n", memory_debug_file); fflush(memory_debug_file);
    if (procfile.fail()) {
        fputs("get_peak_memory_in_kb failed\n", memory_debug_file); fflush(memory_debug_file);
        memory_in_kb = -1;
    }
#endif

    if (memory_in_kb == -1) {
        fputs("get_peak_memory_in_kb memory is still -1\n", memory_debug_file); fflush(memory_debug_file);
        cerr << "warning: could not determine peak memory" << endl;
    }
    fputs("Exit get_peak_memory_in_kb\n", memory_debug_file); fflush(memory_debug_file);
    return memory_in_kb;
}

void print_peak_memory() {
    fputs("Enter print_peak_memory\n", memory_debug_file); fflush(memory_debug_file);
    cout << "Peak memory: " << get_peak_memory_in_kb() << " KB" << endl;
    fputs("Exit print_peak_memory\n", memory_debug_file); fflush(memory_debug_file);
}
