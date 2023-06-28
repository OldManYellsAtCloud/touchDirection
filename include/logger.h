#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>
#include <string>

#define LOG(LOGLEVEL, MSG)  Logger::getLogger().log((LOGLEVEL), (MSG))

class Logger
{
public:
    static Logger& getLogger(int logLevel = LOG_INFO){
        static Logger logger {logLevel};
        return logger;
    }

    void log(int logLevel, std::string msg);

    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;
private:
    Logger(int logLevel);
};

#endif // LOGGER_H
