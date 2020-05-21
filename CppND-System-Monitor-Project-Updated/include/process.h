#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();                          
  std::string User();                   
  std::string Command();                 
  float CpuUtilization();               
  void CalcCpuUtilization();
  std::string Ram();                     
  long int UpTime();                    
  bool operator<(Process const& a) const; 
  bool operator==(Process const& a) const;

 private:
    int _pid = 0;
    long _prev_active;              //prev active jiffies since start of proc
    long _prev_total;               //prev jiffies since start of system
    float _util = -1;
};

#endif