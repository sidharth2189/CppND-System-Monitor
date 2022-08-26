#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;
using std::unordered_map;

// Conversion between KiloBytes and MegoBytes
#define KB_MB 1000.0

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
  string os, kernel, version;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  /* 
    MemCached = Cached + SReclaimable - Shmem
    MemUtilization = 100*(MemTotal - MemFree - (Buffers + MemCached))/MemTotal
  */
  string line, key, value, unit;
  unordered_map<string,float> meminfo;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal" || key == "MemFree" || key == "Buffers" \
        || key == "Cached" || key == "SReclaimable" || key == "Shmem") {
          meminfo[key] = stof(value)/KB_MB; // value in MB
        }
      }
    }
  }
  if (meminfo["MemTotal"] != 0) {
    return (meminfo["MemTotal"] - meminfo["MemFree"] - (meminfo["Buffers"] \
    + meminfo["Cached"] + meminfo["SReclaimable"] - meminfo["Shmem"]))/meminfo["MemTotal"];
  }
  return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line, uptime, idle;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
  }
  return stol(uptime);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, cpuTime;
  vector<string> cpuTimes;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> cpuTime) {
      if (cpuTime != "cpu") { cpuTimes.push_back(cpuTime); }
    }
  }
  return cpuTimes;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") { return stoi(value); }
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") { return stoi(value); }
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string p_id{"/" + to_string(pid) + "/"};
  string line;
  std::ifstream filestream(kProcDirectory + p_id + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    return linestream.str();
  }
  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string p_id{"/" + to_string(pid) + "/"};
  string line, key, value;
  std::ifstream filestream(kProcDirectory + p_id + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          long value_kb = stol(value);
          long value_mb = value_kb/KB_MB; // Value in MB
          return to_string(value_mb);
        }
      }
    }
  }
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string p_id{"/" + to_string(pid) + "/"};
  string line, key, value;
  std::ifstream filestream(kProcDirectory + p_id + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") { return value; }
      }
    }
  }
  return string(); 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid = LinuxParser::Uid(pid);
  string line, userInfo;
  vector<string> userInfos;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> userInfo) {
        userInfos.push_back(userInfo);
        if (userInfo == uid) { return userInfos[0];}
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string p_id{"/" + to_string(pid) + "/"};
  string line, statInfo;
  long upTime = 0;
  vector<string> statInfos;
  std::ifstream filestream(kProcDirectory + p_id + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> statInfo) { statInfos.push_back(statInfo); }
  }
  if (statInfos.size() > 21) { upTime = (stol(statInfos[21]))/sysconf(_SC_CLK_TCK); }
  return upTime;
}

// Read and return the cpu utilization of a process
vector<string> LinuxParser::CpuUtilization(int pid) { 
  string p_id{"/" + to_string(pid) + "/"};
  string line, statInfo;
  vector<string> statInfos;
  std::ifstream filestream(kProcDirectory + p_id + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> statInfo) { statInfos.push_back(statInfo); }
  }
  return statInfos;
}
