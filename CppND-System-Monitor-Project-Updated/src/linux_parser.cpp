#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include <iostream>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Return Operating System
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Return Kernel name
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Return all active process PIDs
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string key, value, line;
  float MemTotal=0, MemFree=0, MemAvailable=0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "MemTotal:")  MemTotal = stof(value);
        if(key == "MemFree:") MemFree = stof(value);
        if(key == "MemAvailable:") {
          MemAvailable = stof(value);
          break;
        }
      }
    }
  }
   
  return (MemTotal-MemFree-MemAvailable)/MemTotal; 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long t_uptime=-1, t_idle;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> t_uptime >> t_idle;
  }
  return t_uptime; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long user, nice, system, idle, iowait, irq, sirq, steal, guest, guest_nice;
  string line, cpu;
  std::ifstream stream (kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> sirq >> steal >> guest >> guest_nice;
  } 
  long total = user + nice + system + idle + iowait + irq + sirq + steal + guest + guest_nice;
  return total;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long utime=0, stime=0, cutime=0, cstime=0;
  int i;
  string line, s;
  string filepath = kProcDirectory + to_string(pid) + "/" + kStatFilename;
  std::ifstream stream(filepath);
  if(stream.is_open()){
    std::getline(stream, line);
    std::stringstream linestream(line);
    for(i=1; i<15; i++){
      switch (i)
      {
      case 14: linestream >> utime >> stime >> cutime >> cstime;
        break;
      default:linestream >> s;
      if(s == "Content)") i--;                      //browsers have (Web Content) as one field in the stat file
        break;                                      //white space is used as a delimiter, gets thrown off by that
      }
    }
  }
  return (utime+stime+cutime+cstime);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long user, nice, system, idle, iowait, irq, sirq, steal, guest, guest_nice;
  string line, cpu;
  std::ifstream stream (kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> sirq >> steal >> guest >> guest_nice;
  } 
  long total = user + nice + system + idle + iowait + irq + sirq + steal + guest + guest_nice;
  return total-idle-iowait;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long user, nice, system, idle, iowait, irq, sirq, steal, guest, guest_nice;
  string line, cpu;
  std::ifstream stream (kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> sirq >> steal >> guest >> guest_nice;
  } 
  return idle+iowait;
}

// Read and return CPU utilization
float LinuxParser::CpuUtilization() {
  static long prev_active = ActiveJiffies();
  static long prev_total = 0;
  long active=ActiveJiffies();
  long total = Jiffies();

  long active_diff = active-prev_active;
  long total_diff = total-prev_total;
  float util = active_diff*1.0/total_diff;

  prev_active = active;
  prev_total = total;
  return util;
}

float LinuxParser::CpuUtilization(int pid) {
    static long prev_active_pid = 0;
    static long prev_total_pid = 0;
    long active = LinuxParser::ActiveJiffies(pid);
    long total = LinuxParser::Jiffies();

    long active_diff = active - prev_active_pid;
    long total_diff = total - prev_total_pid;
    float util = active_diff*1.0/total_diff;

    prev_active_pid = active;
    prev_total_pid = total;

    return util;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int value;
  string line, key;
  std::ifstream stream (kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::stringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "processes") return value;
      }
    }
  }
  return -1;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int value;
  string line, key;
  std::ifstream stream (kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::stringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "procs_running") {return value;}
      }
    }
  }
  return -1;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
    string line="";
    std::ifstream stream (kProcDirectory + to_string(pid)+"/"+kCmdlineFilename);
    if(stream.is_open()){
      getline(stream, line);
      if(line.size() > 102) return line.substr(0, 54);
      else return line + string(102 - line.size(), ' ');            //add padding to remove character overlap in display
    }
    return line;
}
// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, ram="";
  long value;
  std::ifstream stream(kProcDirectory + to_string(pid)+"/"+kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::stringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:") {                                    //use VmSize as an indicator of memory usage
        ram = std::to_string(value/1024);
        if(ram.size()<5) return string(5-ram.size(), ' ') + ram;    //add padding to remove character overlap in display
        else return ram.substr(0, 5);
      }
    }
  }
  return ram;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream stream (kProcDirectory + to_string(pid) + "/" + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::stringstream linestream(line);
      linestream >> key >> value;
      if(key == "Uid:") return value;
    }
  }
  return "error";
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, key, value, x;
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream linestream(line);
      linestream >> key >> x >> value;
      if(value == uid) {
        if(key.size() < 7) return key + string(7-key.size(), ' ');    //add padding to remove character overlap in display
        else return key.substr(0, 7);
        
      }
    }
  }
  return "error";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long starttime, sys_uptime;
  int i;
  string s;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + "/" + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::stringstream linestream(line);
    for(i=0; i<22; i++){
      linestream >> s;
      if(s == "Content") i--;     //browsers have (Web Content) as one field in the stat file
    }
    starttime = stof(s);
    starttime = starttime/sysconf(_SC_CLK_TCK);
    sys_uptime = UpTime();
    if(sys_uptime >= starttime)
    return sys_uptime - starttime;
  }
  return -1;
}