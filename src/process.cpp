#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid)
{
    pid_ = pid;
    cpu_utilization_= Process::CpuUtilization();
    user_ = LinuxParser::User(pid_);
    command_ = LinuxParser::Command(pid_);
}
// DONE: Return this process's ID
int Process::Pid(){ 
    return pid_;
}
// TODO: Return this process's CPU utilization
float Process::CpuUtilization(){
    long l_totalTimeActiveSeconds = LinuxParser::ActiveJiffies(pid_)/sysconf(_SC_CLK_TCK);
    long l_processUpTimeSeconds = LinuxParser::UpTime(pid_);
    cpu_utilization_ = static_cast<float>(static_cast<float>(l_totalTimeActiveSeconds)/static_cast<float>(l_processUpTimeSeconds));
    return (cpu_utilization_);
}

// DONE: Return the command that generated this process
string Process::Command(){
    return command_;
}

// DONE: Return this process's memory utilization
string Process::Ram()
{
    return LinuxParser::Ram(pid_);
}

// DONE: Return the user (name) that generated this process
string Process::User()
{
    return user_;
}

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() 
{
    return LinuxParser::UpTime(pid_);
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator>(Process const& a) const {
    return this->cpu_utilization_ > a.cpu_utilization_; 
}