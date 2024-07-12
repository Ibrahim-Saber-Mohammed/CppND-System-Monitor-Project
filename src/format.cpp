#include <string>

#include "format.h"

//REVIEW: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
std::string Format::ElapsedTime(long seconds) 
{
    constexpr uint16_t SECONDS_PER_HOUR = 60*60;
    constexpr uint16_t SECONDS_PER_MINUTE = 60;
    uint16_t l_hours = seconds/(SECONDS_PER_HOUR);
    long l_remainingSeconds = seconds%(SECONDS_PER_HOUR);
    uint16_t l_minutes = (l_remainingSeconds /SECONDS_PER_MINUTE);
    l_remainingSeconds = (seconds%SECONDS_PER_MINUTE);
    return std::string(std::to_string(l_hours) + ":" + std::to_string(l_minutes) + ":" + std::to_string(l_remainingSeconds)); 
}