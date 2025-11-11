#pragma once

#include <iostream>
#include <string>


inline void DebugLog(std::string category, std::string message){
    std::cout << "[" + category + "]" + " " + message << std::endl;
}