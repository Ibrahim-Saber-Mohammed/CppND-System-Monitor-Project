#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

template<typename T> 
T ParseKeyFromFile(const std::string& Copy_RefFilePath, const std::string& Copy_key) {
  T l_value{};
  std::string l_line{};
  std::string l_key{};
  std::ifstream l_fileStream(Copy_RefFilePath);
  if(l_fileStream.is_open())
  {
    while(std::getline(l_fileStream, l_line))
    {
      std::istringstream l_lineStream{l_line};
      l_lineStream >> l_key ;
      if(l_key == Copy_key)
      {
        l_lineStream >> l_value;
        l_fileStream.close();
        return l_value;
      }
    }
  }
  l_fileStream.close();
  return l_value; 
}


// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_,
  END
};
std::vector<long> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif