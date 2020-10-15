#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace ARAPUCA
{

std::shared_ptr<spdlog::logger> Logger::s_TermLogger;
std::map<std::string, std::shared_ptr<spdlog::logger>> Logger::s_FileLogger;
uint8_t Logger::s_VerboseLevel;

void Logger::Init(const uint8_t &verbose)
{
    s_VerboseLevel = verbose;
    s_TermLogger = spdlog::stdout_color_mt("ArapucaSim");
    s_TermLogger->set_level(spdlog::level::trace);
    s_TermLogger->set_pattern("%^[%T] %n :: %v%$");
}

void Logger::NewLog(const std::string &fileName, const bool &overwrite)
{
    s_FileLogger[fileName] = spdlog::basic_logger_mt(fileName,fileName,overwrite);
    s_FileLogger[fileName]->set_level(spdlog::level::trace);
    s_FileLogger[fileName]->set_pattern("%v%$");
}
} // namespace GX