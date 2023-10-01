#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::map;
using std::set;
using std::size_t;
using std::string;
using std::vector;

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DINE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
   
    vector<int> pids{LinuxParser::Pids()};
    processes_={} ;
    
    // create Set Container 
    set<int> previousPids;
    for(Process const& process : processes_){
        previousPids.insert(process.Pid());
     }

    // Adding new pids objects to processes_
    for (int pid : pids) {
      if (previousPids.find(pid) == previousPids.end())
          processes_.emplace_back(pid);
    }

    // update CPU % utilization
    for (auto& process : processes_){
        process.CpuUtilization();
    }
    // Sort process in Decending order overloading greater than > operator
    std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
    return processes_; 
    }

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() const { return LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() const { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
