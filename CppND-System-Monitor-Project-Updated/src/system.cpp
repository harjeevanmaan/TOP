#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
    vector<int> pids = LinuxParser::Pids();
    for(auto pid : pids){
        if(std::find(processes_.begin(), processes_.end(), Process(pid)) == processes_.end())
            processes_.push_back(Process(pid));
    }
    for(auto& process : processes_){
        process.CalcCpuUtilization();
    }
    std::sort(processes_.begin(), processes_.end());
    return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() {
    return LinuxParser::Kernel();
}

// Return the system's memory utilization
float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

// Return the operating system name
std::string System::OperatingSystem() {
    return LinuxParser::OperatingSystem();
}

// Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long int System::UpTime() {
    long t_uptime = LinuxParser::UpTime();
    return t_uptime;
}