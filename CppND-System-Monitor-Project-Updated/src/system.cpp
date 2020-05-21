#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"
#include <algorithm>

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    vector<int> pids = LinuxParser::Pids();
    vector<Process>::iterator it;
    for(auto pid : pids){
        if(std::find(processes_.begin(), processes_.end(), Process(pid)) == processes_.end())
            processes_.push_back(Process(pid));
    }
    for(auto process : processes_){
        if(process.Pid() == 2547) {
            process.CalcCpuUtilization();}
        else process.CalcCpuUtilization();
    }
    std::sort(processes_.begin(), processes_.end());
    // std::sort(processes_.begin(), processes_.end(), Process::Compare);
    return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {
    return LinuxParser::Kernel();
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

// TODO: Return the operating system name
std::string System::OperatingSystem() {
    return LinuxParser::OperatingSystem();
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() {
    long t_uptime = LinuxParser::UpTime();
    return t_uptime;
}