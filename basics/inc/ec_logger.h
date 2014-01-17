#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <cstdarg>

#include "ec_log.h"

class Logger : public ILog
{
public:
    Logger(const char* log_file_prefix);
    virtual ~Logger();

    virtual void debug(const char* format, va_list& args);
    virtual void info(const char* format, va_list& args);
    virtual void warn(const char* format, va_list& args);
    virtual void error(const char* format, va_list& args);

private:
    void output(const char* file_name_suffix, const char* log_prefix, const char* format, va_list& args);
    void output_level(int level, const char* format, va_list& args);

private:
    const char* _log_file_prefix;
};

#endif
