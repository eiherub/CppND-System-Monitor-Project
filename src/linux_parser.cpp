#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value;
  vector<float> memUsed;
  std::ifstream filemem(kProcDirectory + kMeminfoFilename);
  if (filemem.is_open()) {
    while (std::getline(filemem, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:" || key == "MemFree:") {
          memUsed.emplace_back(value);
        }
      }
    }
  }
  // Used MemTotal - MemFree to determine Memory used by the system and divided
  // by Memtotal to express in decimals
  return (memUsed[0] - memUsed[1]) / memUsed[0];
}
// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  float uptime, suspendtime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> suspendtime;
  }
  return uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return LinuxParser::UpTime()/sysconf(_SC_CLK_TCK) ;
 }

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  vector<string> status;
  double pidCPUptime;
  double cputimeinseconds;

  std::ifstream filepids(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filepids.is_open()) {
    std::getline(filepids, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      status.emplace_back(value);
    }
  }
  pidCPUptime = (stof(status[kUtime_]) + stof(status[kStime_]) + stof(status[kCutime_]) + stof(status[kCstime_])) / sysconf(_SC_CLK_TCK);
  cputimeinseconds = (stof(status[kStarttime_])/sysconf(_SC_CLK_TCK));
  return cputimeinseconds-pidCPUptime;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto value = LinuxParser::CpuUtilization();
  return value[kUser_] + value[kNice_] + value[kSystem_] + value[kIRQ_] +
         value[kSoftIRQ_] + value[kSteal_];
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto value = LinuxParser::CpuUtilization();
  return value[kIdle_] + value[kIOwait_];
}

// DONE: Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() {
  string line;
  string key;
  float value;
  vector<long> cpusused;
  std::ifstream filestat(kProcDirectory + kStatFilename);
  if (filestat.is_open()) {
    while (std::getline(filestat, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          while (linestream >> value) {
            cpusused.emplace_back(value);
          }
        }
      }
    }
  }
  return cpusused;
}


// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value = 0;

  std::ifstream filemem(kProcDirectory + kStatFilename);
  if (filemem.is_open()) {
    while (std::getline(filemem, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value = 0;

  std::ifstream fileproc(kProcDirectory + kStatFilename);
  if (fileproc.is_open()) {
    while (std::getline(fileproc, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmdline;

  std::ifstream fileuser(kProcDirectory + std::to_string(pid) +
                         kCmdlineFilename);
  if (fileuser.is_open()) {
    while (std::getline(fileuser, cmdline)) {
    std::istringstream linestream(cmdline);
      }
  }
  return cmdline;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  long value;

  std::ifstream fileuser(kProcDirectory + std::to_string(pid) +
                         kStatusFilename);
  if (fileuser.is_open()) {
    while (std::getline(fileuser, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "VmSize:") {
          linestream >> value;
        }
      }
    }
  }
  return std::to_string(value/1024);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;

  std::ifstream fileuser(kProcDirectory + std::to_string(pid) +
                         kStatusFilename);
  if (fileuser.is_open()) {
    while (std::getline(fileuser, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "Uid:") {
          linestream >> value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string key;
  string passwd;
  string userid;

  std::ifstream fileuser(kPasswordPath);
  if (fileuser.is_open()) {
    while (std::getline(fileuser, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> passwd >> userid) {
        if (userid == LinuxParser::Uid(pid)) {
          return key;
        }
      }
    }
  }
  return key;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  vector<string> status;
  long pidUptime;
  std::ifstream filepids(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filepids.is_open()) {
    std::getline(filepids, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      status.emplace_back(value);
    }
  }
  pidUptime = stof(status[kStarttime_])/sysconf(_SC_CLK_TCK);
  return pidUptime;  
}
