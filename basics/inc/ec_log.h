#ifndef _LOG_H_
#define _LOG_H_

#include <cstdarg>

class ILog
{
public:
    virtual void debug(const char* format, va_list& args) = 0;
    virtual void info(const char* format, va_list& args) = 0;
    virtual void warn(const char* format, va_list& args) = 0;
    virtual void error(const char* format, va_list& args) = 0;
    virtual ~ILog()
    {
    }
};

void set_active_logger(ILog* logger);
void config_logger(const char* log_prefix);
void log_debug(const char* format, ...);
void log_info(const char* format, ...);
void log_warn(const char* format, ...);
void log_error(const char* format, ...);

#endif
