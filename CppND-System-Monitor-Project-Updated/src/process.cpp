#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;


Process::Process(int pid) : _pid(pid) {
    _prev_active = LinuxParser::ActiveJiffies(_pid);
    _prev_total = LinuxParser::Jiffies();
    CalcCpuUtilization();
  }

// Return this process's ID
int Process::Pid() {
    return _pid;
}

// Return this process's CPU utilization
float Process::CpuUtilization() {
    return _util;
}

void Process::CalcCpuUtilization() {
    long active = LinuxParser::ActiveJiffies(_pid);             //current active jiffies since start of proc
    long total = LinuxParser::Jiffies();                        //current total jiffies since start of system

    long active_diff = active - _prev_active;
    long total_diff = total - _prev_total;
    float util = active_diff*1.0/total_diff;

    _prev_active = active;
    _prev_total = total;
    if(total_diff == 0) {                                   //avoid divide by 0
        _util = 0;
        }
    else _util = sysconf(_SC_NPROCESSORS_ONLN)*util;           //multiply by number of processors for multiprocessor systems
}

// Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(_pid);
}

// Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(_pid);
}

// Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(_pid);
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(_pid);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    if(_util > a._util) return true;
    return false;
}
// Overload the "equal" comparison operator for Process objects
bool Process::operator==(Process const& a) const {
    if(a._pid == _pid) return true;
    return false;
}