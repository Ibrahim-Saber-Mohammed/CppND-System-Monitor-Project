#include "processor.h"
#include "linux_parser.h"


// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
    float l_cpuUsage{}; 
    long l_IdleJiffies{};  
    long l_ActiveJiffies{};
    long l_currTotal{};    
    long l_deltaIdle{};    
    long l_deltaTotal{};   
    
    l_IdleJiffies  = LinuxParser::IdleJiffies(); 
    l_ActiveJiffies = LinuxParser::ActiveJiffies(); 
    l_currTotal     = l_IdleJiffies + l_ActiveJiffies;
    l_deltaIdle     = l_IdleJiffies - m_prevIdle_;
    l_deltaTotal    = l_currTotal - m_prevTotal_;

    m_prevIdle_ = l_IdleJiffies;
    m_prevNonIdle_ = l_ActiveJiffies;
    m_prevTotal_ = l_currTotal;
    
    l_cpuUsage= l_deltaTotal == 0 ? 0.0f : (l_deltaTotal - l_deltaIdle)*1.0/l_deltaTotal;
    return l_cpuUsage;
}