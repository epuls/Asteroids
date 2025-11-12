#pragma once

#include <iostream>
#include <string>
#include <format>



class Debug{
public:
    enum LogLevel{
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        NONE
    };

    static int LoggingLevel;


    static void Log(const std::string category, const std::string message){
        if (LoggingLevel > 0) return;
        Record(category, message);

    }

    static void LogInfo(const std::string category, const std::string message){
        if (LoggingLevel > 1) return;
        Record(category, message);
    }

    static void LogWarning(const std::string category, const std::string message){
        if (LoggingLevel > 2) return;
        Record(category, message);
    }

    static void LogError(std::string category, std::string message){
        if (LoggingLevel > 3) return;
        Record(category, message);
    }

private:
    static void Record(const std::string& category, const std::string& message){
        std::time_t t = std::time(nullptr);
        std::tm tm{};
#if __WIN32__
        localtime_s(&tm, &t);
#elif __APPLE__
        localtime_r(&tm, &t);
#endif
        std::string dateTime = std::format("[{:02d}:{:02d}:{:02d}] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
        std::cout << dateTime << "[" + category + "]" + " " + message << std::endl;
    }

};


// -------- Compile-time logging config --------
#define LOG_LEVEL_DEBUG   0
#define LOG_LEVEL_INFO    1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR   3
#define LOG_LEVEL_NONE    4

#ifndef BUILD_LOG_LEVEL
#define BUILD_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

// -------- Per-level macros (compiled out when disabled) --------
#if BUILD_LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(category, message) do { ::Debug::Log((category), (message)); } while(0)
#else
#define LOG_DEBUG(category, message) ((void)0)
#endif

#if BUILD_LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(category, message) do { ::Debug::LogInfo((category), (message)); } while(0)
#else
#define LOG_INFO(category, message) ((void)0)
#endif

#if BUILD_LOG_LEVEL <= LOG_LEVEL_WARNING
#define LOG_WARNING(category, message) do { ::Debug::LogWarning((category), (message)); } while(0)
#else
#define LOG_WARNING(category, message) ((void)0)
#endif

#if BUILD_LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR(category, message) do { ::Debug::LogError((category), (message)); } while(0)
#else
#define LOG_ERROR(category, message) ((void)0)
#endif
