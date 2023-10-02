#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

// DONE: Return this process's ID
int Process::Pid() const { return pid_; }


// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {

  float timeActive = LinuxParser::ActiveJiffies(Pid()); //
  float cputimeActive = LinuxParser::ActiveJiffies() - LinuxParser::IdleJiffies();
  float timeInseconds = LinuxParser::UpTime(Pid())/LinuxParser::Jiffies()  ; 
  float totalTimeElapse = (timeActive - cputimeActive) / timeInseconds; 

  return static_cast<float>(timeActive)/totalTimeElapse;
 } 

// DONE: Return the command that generated this process
string Process::Command() { 
  int const cmdlineMax{50};
  string cmdline = LinuxParser::Command(Pid());
  if (cmdline.size() > cmdlineMax){
    cmdline.resize(cmdlineMax);
    cmdline=cmdline +"...";
  }

  return  cmdline ;}

// DONE: Return this process's memory utilization
string Process::Ram()  { return LinuxParser::Ram(Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime() - LinuxParser::UpTime(Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
// bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }

bool Process::operator<(Process const& a) const { return a.CpuUtilization() < CpuUtilization(); }
