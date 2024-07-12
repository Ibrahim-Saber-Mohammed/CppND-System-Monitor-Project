#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  std::string l_line{};
  std::string l_key{};
  std::string l_value{};
  /* open the file containing the Os release info */
  std::ifstream Osfile(kOSPath);
  /* Check if the file is open successfully */
  if (Osfile.is_open()) {
    /* read the file line by line */
    while (std::getline(Osfile, l_line)) {
      /* split the line into key and value */
      std::replace(l_line.begin(), l_line.end(), ' ', '_');
      std::replace(l_line.begin(), l_line.end(), '=', ' ');
      std::replace(l_line.begin(), l_line.end(), '"', ' ');
      /* Read the Line and extract the Key and the Value */
      std::istringstream L_lineStream{l_line};
      while (L_lineStream >> l_key >> l_value) {
        /*if we Found the PRETTY_NAME key*/
        if (l_key == "PRETTY_NAME") {
          /* return the value representing the Os name*/
          std::replace(l_value.begin(), l_value.end(), '_', ' ');
          Osfile.close();
          return l_value;
        }
      }
    }
  }
  Osfile.close();
  return l_value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  std::string l_line{};
  std::string l_Os{};
  std::string l_version{};
  std::string l_kernel{};
  std::ifstream l_kernelInfoFile(kProcDirectory + kVersionFilename);
  if (l_kernelInfoFile.is_open()) {
    std::getline(l_kernelInfoFile, l_line);
    std::istringstream l_lineStream{l_line};
    l_lineStream >> l_Os >> l_version >> l_kernel;
  }
  l_kernelInfoFile.close();
  return l_kernel;
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

// REVIEW: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  /* declaring the Local variables*/
  float l_SystemTotalMemory{};
  float l_SystemFreeMemory{};
  float l_SystemUsedMemory{};
  /* Declare the Path to the meminfo file */
  const std::string l_memoryInfoPath{kProcDirectory + kMeminfoFilename};
  /* read the total memory from the memory info file */
  l_SystemTotalMemory = ParseKeyFromFile<float>(l_memoryInfoPath, "MemTotal:");
  /* read the free memory from the memory info file */
  l_SystemFreeMemory = ParseKeyFromFile<float>(l_memoryInfoPath, "MemFree:");
  l_SystemUsedMemory = (l_SystemTotalMemory - l_SystemFreeMemory);
  /* Check the total memory not equal to zero and return the used memory
   * percentage */
  l_SystemUsedMemory = (l_SystemTotalMemory == 0.0f)
                           ? 0.0f
                           : (l_SystemUsedMemory / l_SystemTotalMemory);
  return l_SystemUsedMemory;
}

// REVIEW: Read and return the system uptime
long LinuxParser::UpTime() {
  long l_SystemUptime{};
  std::ifstream l_SystemUptimestream(kProcDirectory + kUptimeFilename);
  if (l_SystemUptimestream.is_open()) {
    l_SystemUptimestream >> l_SystemUptime;
  }
  l_SystemUptimestream.close();
  return l_SystemUptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long l_ProcessActiveJiffies{};
  std::string l_Garbagevalue{};
  long l_userTimeTicks{};
  long l_kernelTimeTicks{};
  long l_userChildrenTimeTicks{};
  long l_kernelChildrenTimeTicks{};

  std::string l_ProcessStatFilePath{kProcDirectory + std::to_string(pid) +
                                    kStatFilename};
  std::ifstream l_ProcessStatFileStream(l_ProcessStatFilePath);
  if (l_ProcessStatFileStream.is_open()) {
    constexpr int l_ProcessActiveJiffiesPosition{13};
    for (int l_loopIterator{0}; l_loopIterator < l_ProcessActiveJiffiesPosition;
         l_loopIterator++) {
      if (!(l_ProcessStatFileStream >> l_Garbagevalue)) {
        return 10000;
      }
    }
    if (l_ProcessStatFileStream >> l_userTimeTicks >> l_kernelTimeTicks >>
        l_userChildrenTimeTicks >> l_kernelChildrenTimeTicks) {
      l_ProcessActiveJiffies =
          (l_userTimeTicks + l_kernelTimeTicks + l_userChildrenTimeTicks +
           l_kernelChildrenTimeTicks);
    }
  }
  return (l_ProcessActiveJiffies / sysconf(_SC_CLK_TCK));
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<long> l_VectorCpuJiffies = CpuUtilization();

  return (l_VectorCpuJiffies[kUser_] + l_VectorCpuJiffies[kNice_] + l_VectorCpuJiffies[kSystem_] +
         l_VectorCpuJiffies[kIRQ_] + l_VectorCpuJiffies[kSoftIRQ_] + l_VectorCpuJiffies[kSteal_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<long> l_VectorCpuJiffies = CpuUtilization();
  return (l_VectorCpuJiffies[kIdle_] + l_VectorCpuJiffies[kIOwait_]);
}

// DONE: Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() {
  std::vector<long> l_cpuJiffies;
  std::string cpu{};
  std::ifstream l_SystemStatFileStream(kProcDirectory + kStatFilename);
  if (l_SystemStatFileStream.is_open()) {
    l_SystemStatFileStream >> cpu;
    if (cpu == "cpu") {
      for (int i = 0; i < CPUStates::END; i++) {
        long value;
        l_SystemStatFileStream >> value;
        l_cpuJiffies.push_back(value);
      }
    }
  }
  l_SystemStatFileStream.close();
  return l_cpuJiffies;
}

// REVIEW: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int l_TotalProcesses{};
  std::string l_SystemStatFile{kProcDirectory + kStatFilename};
  l_TotalProcesses = ParseKeyFromFile<int>(l_SystemStatFile, "processes");
  return l_TotalProcesses;
}

// REVIEW: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int l_RunningProcesses{};
  std::string l_SystemStatFile{kProcDirectory + kStatFilename};
  l_RunningProcesses = ParseKeyFromFile<int>(l_SystemStatFile, "procs_running");
  return l_RunningProcesses;
}

// REVIEW: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string l_line;
  std::string l_processCommand{"NA"};
  std::string l_processFilePath{kProcDirectory + std::to_string(pid) +
                                kCmdlineFilename};
  std::ifstream l_processFileStream(l_processFilePath);
  if (l_processFileStream.is_open()) {
    while (std::getline(l_processFileStream, l_line)) {
      std::istringstream l_lineStream{l_line};
      std::replace(l_line.begin(), l_line.end(), ' ', '*');
      while(l_lineStream >> l_processCommand)
      {
        std::replace(l_line.begin(), l_line.end(), '*', ' ');
        return l_processCommand;
      }
    }
  }
  l_processFileStream.close();
  return l_processCommand;
}

// REVIEW: Read and return the memory used by a process
std::string LinuxParser::Ram(int pid) {
  constexpr uint16_t MGB_KB_FACTOR{1024};
  int l_ProcessRamUsageKB{};
  std::string l_ProcessRamUsageMB{};
  std::string l_ProcessStatusFilePath{kProcDirectory + std::to_string(pid) +
                                      kStatusFilename};
  l_ProcessRamUsageKB = ParseKeyFromFile<int>(l_ProcessStatusFilePath, "VmSize:");
  l_ProcessRamUsageMB = std::to_string(l_ProcessRamUsageKB / MGB_KB_FACTOR);
  return l_ProcessRamUsageMB;
}

// REVIEW: Read and return the user ID associated with a process
std::string LinuxParser::Uid(int pid) {
  std::string l_ProcessUid{"0000"};
  std::string l_ProcessStatusFilePath{kProcDirectory + std::to_string(pid) +
                                      kStatusFilename};
  l_ProcessUid = ParseKeyFromFile<std::string>(l_ProcessStatusFilePath, "Uid:");
  return l_ProcessUid;
}

// REVIEW: Read and return the user associated with a process
std::string LinuxParser::User(int pid) {
  std::string l_UserPasswdFilePath{kPasswordPath};
  std::string l_line{};
  std::string l_UserName{""};
  std::string l_xField{};
  std::string l_ChangedUid{};
  std::string l_UserUid{Uid(pid)};
  std::ifstream l_PasswdFileStream(l_UserPasswdFilePath);
  if (l_PasswdFileStream.is_open()) {
    while (std::getline(l_PasswdFileStream, l_line)) {
      std::replace(l_line.begin(), l_line.end(), ':', ' ');
      std::istringstream l_lineStream{l_line};
      if (l_lineStream >> l_UserName >> l_xField >> l_ChangedUid) {
        if (l_UserUid == l_ChangedUid) {
          l_PasswdFileStream.close();
          return l_UserName;
        }
      }
    }
  }
  l_PasswdFileStream.close();
  return l_UserName;
}
// REVIEW: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string l_ProcessStatFilePath{kProcDirectory + std::to_string(pid) + kStatFilename};
  std::string l_line{};
  std::string l_value{};
  long l_ProcessStartTime{};
  std::ifstream l_ProcessStatStream(l_ProcessStatFilePath);
  if (l_ProcessStatStream.is_open()) {
    constexpr int l_ProcessStartTimePosition = 22;
    for (int l_loopIterator = 0; l_loopIterator < l_ProcessStartTimePosition;
         l_loopIterator++) {
      if (!(l_ProcessStatStream >> l_value)) {
        return 10000;
      }
    }
    l_ProcessStartTime = std::stol(l_value);
  }
  l_ProcessStartTime = (l_ProcessStartTime / sysconf(_SC_CLK_TCK));
  return (LinuxParser::UpTime() - l_ProcessStartTime);
}
