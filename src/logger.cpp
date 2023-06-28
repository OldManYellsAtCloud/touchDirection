#include "logger.h"

void Logger::log(int logLevel, std::string msg)
{
    syslog(logLevel, "%s", msg.c_str());
}

Logger::Logger(int logLevel)
{
    setlogmask(LOG_UPTO(logLevel));
    openlog(NULL, LOG_PID, LOG_LOCAL1);
}
