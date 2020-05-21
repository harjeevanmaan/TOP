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
// bool Compare(RouteModel::Node* n1, RouteModel::Node* n2){
//     float f1 = n1->g_value + n1->h_value;
//     float f2 = n2->g_value + n2->h_value;

//     return (f1>f2);
// }
// static bool Process::Compare(Process* p1, Process* p2){
//     return (p1->_ram > p2->_ram);
// }

Process::Process(int pid) : _pid(pid) {
    _prev_active = LinuxParser::ActiveJiffies(_pid);
    _prev_total = LinuxParser::Jiffies();
    CalcCpuUtilization();
    // _ram = LinuxParser::Ram(_pid);
  }

// TODO: Return this process's ID
int Process::Pid() {
    return _pid;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    // CalcCpuUtilization();
    return _util;
}

void Process::CalcCpuUtilization() {
    long active = LinuxParser::ActiveJiffies(_pid);
    long total = LinuxParser::Jiffies();

    long active_diff = active - _prev_active;
    long total_diff = total - _prev_total;
    float util = active_diff*1.0/total_diff;
    if(total_diff == 0) {
        _util = 0;
        return;
    }

    _prev_active = active;
    _prev_total = total;
    _util = sysconf(_SC_NPROCESSORS_ONLN)*util;
}

// TODO: Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(_pid);
}

// TODO: Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(_pid);
}

// TODO: Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(_pid);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(_pid);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    if(_util > a._util) return true;
    return false;
}

bool Process::operator==(Process const& a) const {
    if(a._pid == _pid) return true;
    return false;
}