#include "utilities.h"

#include <cassert>
#include <csignal>
#include <iostream>
#include <fstream>
#include <limits>
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
static std::fstream memory_debug_fs;

static void out_of_memory_handler();
static void signal_handler(int signal_number);


void register_event_handlers() {
    memory_debug_fs.open("memory_debug.log", ios_base::out);
    // When running out of memory, release some emergency memory and
    // terminate.
    set_new_handler(out_of_memory_handler);
    memory_debug_fs.write("Handlers set\n");
    memory_debug_fs.flush();

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
    memory_debug_fs.write("Enter exit_handler\n");
    memory_debug_fs.flush();
    print_peak_memory();
    memory_debug_fs.write("Exit exit_handler\n");
    memory_debug_fs.flush();
    memory_debug_fs.close();
}
#endif

void exit_with(ExitCode exitcode) {
    memory_debug_fs.write("Enter exit_with\n");
    memory_debug_fs.flush();
    switch (exitcode) {
    case EXIT_PLAN_FOUND:
        memory_debug_fs.write("exit_with case 1\n");
        memory_debug_fs.flush();
        cout << "Solution found." << endl;
        break;
    case EXIT_CRITICAL_ERROR:
        memory_debug_fs.write("exit_with case 2\n");
        memory_debug_fs.flush();
        cerr << "Unexplained error occurred." << endl;
        break;
    case EXIT_INPUT_ERROR:
        memory_debug_fs.write("exit_with case 3\n");
        memory_debug_fs.flush();
        cerr << "Usage error occurred." << endl;
        break;
    case EXIT_UNSUPPORTED:
        memory_debug_fs.write("exit_with case 4\n");
        memory_debug_fs.flush();
        cerr << "Tried to use unsupported feature." << endl;
        break;
    case EXIT_UNSOLVABLE:
        memory_debug_fs.write("exit_with case 5\n");
        memory_debug_fs.flush();
        cout << "Task is provably unsolvable." << endl;
        break;
    case EXIT_UNSOLVED_INCOMPLETE:
        memory_debug_fs.write("exit_with case 6\n");
        memory_debug_fs.flush();
        cout << "Search stopped without finding a solution." << endl;
        break;
    case EXIT_OUT_OF_MEMORY:
        memory_debug_fs.write("exit_with case 7\n");
        memory_debug_fs.flush();
        cout << "Memory limit has been reached." << endl;
        break;
    case EXIT_TIMEOUT:
        memory_debug_fs.write("exit_with case 8\n");
        memory_debug_fs.flush();
        cout << "Time limit has been reached." << endl;
        break;
    default:
        memory_debug_fs.write("exit_with default case\n");
        memory_debug_fs.flush();
        cerr << "Exitcode: " << exitcode << endl;
        ABORT("Unkown exitcode.");
    }
    memory_debug_fs.write("Exit exit_with\n");
    memory_debug_fs.flush();
    exit(exitcode);
}

static void out_of_memory_handler() {
    memory_debug_fs.write("Enter out_of_memory_handler\n");
    memory_debug_fs.flush();
    assert(memory_padding);
    memory_debug_fs.write("out_of_memory_handler after assertion\n");
    memory_debug_fs.flush();
    delete[] memory_padding;
    memory_padding = 0;
    memory_debug_fs.write("out_of_memory_handler after delete\n");
    memory_debug_fs.flush();
    cout << "Failed to allocate memory. Released memory buffer." << endl;
    memory_debug_fs.write("Exit out_of_memory_handler\n");
    memory_debug_fs.flush();
    exit_with(EXIT_OUT_OF_MEMORY);
}

void signal_handler(int signal_number) {
    memory_debug_fs.write("Enter signal_handler\n");
    memory_debug_fs.flush();
    // See glibc manual: "Handlers That Terminate the Process"
    static volatile sig_atomic_t handler_in_progress = 0;
    memory_debug_fs.write("signal_handler before if-statement\n");
    memory_debug_fs.flush();
    if (handler_in_progress) {
        memory_debug_fs.write("signal_handler within if-statement\n");
        memory_debug_fs.flush();
        raise(signal_number);
    }
    memory_debug_fs.write("signal_handler after if-statement\n");
    memory_debug_fs.flush();
    handler_in_progress = 1;
    memory_debug_fs.write("signal_handler before print_peak_memory\n");
    memory_debug_fs.flush();
    print_peak_memory();
    memory_debug_fs.write("signal_handler after print_peak_memory\n");
    memory_debug_fs.flush();
    cout << "caught signal " << signal_number << " -- exiting" << endl;
    memory_debug_fs.write("signal_handler after cout\n");
    memory_debug_fs.flush();
    signal(signal_number, SIG_DFL);
    memory_debug_fs.write("Exit signal_handler\n");
    memory_debug_fs.flush();
    memory_debug_fs.close();
    raise(signal_number);
}

int get_peak_memory_in_kb() {
    memory_debug_fs.write("Enter get_peak_memory_in_kb\n");
    memory_debug_fs.flush();
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
    memory_debug_fs.write("get_peak_memory_in_kb create stream\n");
    memory_debug_fs.flush();
    ifstream procfile("/proc/self/status");
    string word;
    memory_debug_fs.write("get_peak_memory_in_kb before loop\n");
    memory_debug_fs.flush();
    while (procfile.good()) {
        procfile >> word;
        memory_debug_fs.write("get_peak_memory_in_kb inside loop\n");
        memory_debug_fs.flush();
        if (word == "VmPeak:") {
            memory_debug_fs.write("get_peak_memory_in_kb found VmPeak\n");
            memory_debug_fs.flush();
            procfile >> memory_in_kb;
            memory_debug_fs.write("get_peak_memory_in_kb read peak mem\n");
            memory_debug_fs.flush();
            break;
        }
        memory_debug_fs.write("get_peak_memory_in_kb before ignore\n");
        memory_debug_fs.flush();
        // Skip to end of line.
        procfile.ignore(numeric_limits<streamsize>::max(), '\n');
        memory_debug_fs.write("get_peak_memory_in_kb after ignore\n");
        memory_debug_fs.flush();
    }
    memory_debug_fs.write("get_peak_memory_in_kb check for fail\n");
    memory_debug_fs.flush();
    if (procfile.fail()) {
        memory_debug_fs.write("get_peak_memory_in_kb failed\n");
        memory_debug_fs.flush();
        memory_in_kb = -1;
    }
#endif

    if (memory_in_kb == -1) {
        memory_debug_fs.write("get_peak_memory_in_kb memory is still -1\n");
        memory_debug_fs.flush();
        cerr << "warning: could not determine peak memory" << endl;
    }
    memory_debug_fs.write("Exit get_peak_memory_in_kb\n");
    memory_debug_fs.flush();
    return memory_in_kb;
}

void print_peak_memory() {
    memory_debug_fs.write("Enter print_peak_memory\n");
    memory_debug_fs.flush();
    cout << "Peak memory: " << get_peak_memory_in_kb() << " KB" << endl;
    memory_debug_fs.write("Exit print_peak_memory\n");
    memory_debug_fs.flush();
}
