#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

/*Default Constructor*/
System::System(void): m_KernelVersion_{LinuxParser::Kernel()}, m_OperatingSystem_(LinuxParser::OperatingSystem()){}

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() 
{ 
    std::set<int> uniqueIds;
    for (Process process : processes_) {
        uniqueIds.insert(process.Pid());
    }
    std::vector<int> l_processesIds = LinuxParser::Pids();
    for (int id: l_processesIds) {
        if (uniqueIds.find(id) == uniqueIds.end()) {
            processes_.emplace_back(Process(id));
        }
    }
    std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
    return processes_; 
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel()
{
    return this->m_KernelVersion_;
}

// DONE: Return the system's memory utilization
float System::MemoryUtilization() 
{
    return LinuxParser::MemoryUtilization();
}

// DONE: Return the operating system name
std::string System::OperatingSystem()
{ 
    return this->m_OperatingSystem_;
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses(){
    return LinuxParser::RunningProcesses();
}

// DONE: Return the total number of processes on the system
int System::TotalProcesses()
{
    return LinuxParser::TotalProcesses();
}

// DONE: Return the number of seconds since the system started running
long int System::UpTime() 
{ 
    return LinuxParser::UpTime();
}