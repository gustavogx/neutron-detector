#pragma once

#include <memory>
#include <map>
#include <string>
#include <cstdlib>

#include "spdlog/spdlog.h"


namespace ARAPUCA{

enum Verbosity { kSilent, kWarning, kAll};

class Logger{
private:

    static std::shared_ptr<spdlog::logger> s_TermLogger;
    static std::map<std::string, std::shared_ptr<spdlog::logger> > s_FileLogger;
    static uint8_t s_VerboseLevel;
    
public:

    static void Init(const uint8_t &verbose=0);
    static inline void SetVerbose(const uint8_t &verbose) { s_VerboseLevel = verbose; }
    static void NewLog(const std::string &name, const bool &overwrite = true);

    inline static std::shared_ptr<spdlog::logger>& GetTermLogger() {return s_TermLogger;}
    inline static std::shared_ptr<spdlog::logger>& GetFileLogger(const std::string &aLogger) {return s_FileLogger[aLogger];}

    inline static bool VerboseLevel(const uint8_t &level=0) { return s_VerboseLevel>=level; }

};

}

#ifndef ARAPUCA_NOLOG

    #define LOG_TERM_TRACE(...) { if(ARAPUCA::Logger::VerboseLevel(ARAPUCA::kAll))   ARAPUCA::Logger::GetTermLogger()->trace(__VA_ARGS__); }
    #define LOG_TERM_INFO(...)  { if(ARAPUCA::Logger::VerboseLevel(ARAPUCA::kAll))    ARAPUCA::Logger::GetTermLogger()->info(__VA_ARGS__); }
    #define LOG_TERM_WARN(...)  { if(ARAPUCA::Logger::VerboseLevel(ARAPUCA::kWarning)) ARAPUCA::Logger::GetTermLogger()->warn(__VA_ARGS__); }
    #define LOG_TERM_ERROR(...) { if(ARAPUCA::Logger::VerboseLevel(ARAPUCA::kSilent))  ARAPUCA::Logger::GetTermLogger()->error(__VA_ARGS__); }
    #define LOG_TERM_FATAL(...) { if(ARAPUCA::Logger::VerboseLevel(ARAPUCA::kSilent))  ARAPUCA::Logger::GetTermLogger()->error(__VA_ARGS__); std::exit(EXIT_FAILURE); }

    #define LOG_FILE(x,...) ARAPUCA::Logger::GetFileLogger(x)->trace(__VA_ARGS__)

#else

    #define LOG_TERM_TRACE
    #define LOG_TERM_INFO
    #define LOG_TERM_WARN
    #define LOG_TERM_ERROR
    #define LOG_TERM_FATAL
    #define LOG_TERM_CRITICAL

#endif