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

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string key, value, line;
  float MemTotal, MemFree, MemAvailable;
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

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long t_uptime, t_idle;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> t_uptime >> t_idle;
  }
  return t_uptime; 
}

// TODO: Read and return the number of jiffies for the system
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
    // std::cout << std::endl << total << std::endl;
  return total;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long j1, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, utime, stime, cutime, cstime;
  string line, s2, s3, s4;
  string filepath = kProcDirectory + to_string(pid) + "/" + kStatFilename;
  std::ifstream stream(filepath);
  if(stream.is_open()){
    std::getline(stream, line);
    std::stringstream linestream(line);
    linestream >> j1 >> s2 >> s3 >> j4 >> j5 >> j6 >> j7 >> j8 >> j9 >> j10 >> j11 >> j12 >> j13 >> utime >> stime >> cutime >> cstime;
    if(s3 == "Content)") linestream >> j1 >> s2 >> s3 >> s4 >> j4 >> j5 >> j6 >> j7 >> j8 >> j9 >> j10 >> j11 >> j12 >> j13 >> utime >> stime >> cutime >> cstime;
  }
  return (utime+stime+cutime+cstime);
}

// TODO: Read and return the number of active jiffies for the system
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

// TODO: Read and return the number of idle jiffies for the system
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

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
  static long prev_active = ActiveJiffies();
  static long prev_total = 0;
  // usleep(50000);
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
    // usleep(10000);
    // if(pid != 1) return 0;
    long active = LinuxParser::ActiveJiffies(pid);
    long total = LinuxParser::Jiffies();

    long active_diff = active - prev_active_pid;
    long total_diff = total - prev_total_pid;
    // if(pid==1)
    // std::cout << std::endl << prev_total_pid << " " << total << " " << prev_active_pid << " " << active << std::endl;
    float util = active_diff*1.0/total_diff;

    prev_active_pid = active;
    prev_total_pid = total;
    // if(pid==1)
    // std::cout << prev_total_pid << " " << total << " " << prev_active_pid << " " << active << std::endl;

    return util;
}

// TODO: Read and return the total number of processes
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

// TODO: Read and return the number of running processes
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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
    string line;
    std::ifstream stream (kProcDirectory + to_string(pid)+"/"+kCmdlineFilename);
    if(stream.is_open()){
      getline(stream, line);
    }
    return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key;
  long value;
  std::ifstream stream(kProcDirectory + to_string(pid)+"/"+kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::stringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:") return std::to_string(value/1024);
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, key, value, x;
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream linestream(line);
      linestream >> key >> x >> value;
      if(value == uid) return key; 
    }
  }
  return "error";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long long j1, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, j14, j15, j16, j17, j18, j19, j20, j21, starttime; 
  string s2, s3, s4;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + "/" + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::stringstream linestream(line);
    linestream >> j1 >> s2 >> s3 >> j4 >> j5 >> j6 >> j7 >> j8 >> j9 >> j10 >> j11 >> j12 >> j13 >> j14 >> j15 >> j16 >> j17 >> j18 >> j19 >> j20 >> j21 >> starttime;
    if(s3 == "Content)") linestream >> j1 >> s2 >> s3 >> s4 >> j4 >> j5 >> j6 >> j7 >> j8 >> j9 >> j10 >> j11 >> j12 >> j13 >> j14 >> j15 >> j16 >> j17 >> j18 >> j19 >> j20 >> j21 >> starttime;
    starttime = starttime/sysconf(_SC_CLK_TCK);
    long sys_uptime = UpTime();
    if(sys_uptime > starttime)
    return sys_uptime - starttime;
  }
  return -1;
}