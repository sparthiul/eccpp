#include "ec_logger.h"

#include <cstdio>
#include <cassert>
#include <string>

using namespace std;

static const char* c_file_name_suffixes[] = {"_debug.log", "_info.log", "_warn.log", "_error.log"};
static const char* c_log_prefixes[]       = {"DEBUG: ",    "INFO: ",    "WARN: ",    "ERROR: "   };

Logger::Logger(const char* log_file_prefix) :
    _log_file_prefix(log_file_prefix)
{
    assert(log_file_prefix != NULL);
}

Logger::~Logger()
{
}

void Logger::output(const char* file_name_suffix, const char* log_prefix, const char* format, va_list& args)
{
    string file_name(this->_log_file_prefix);
    file_name.append(file_name_suffix);
    FILE* file = fopen(file_name.c_str(), "a+");
    if (file == NULL)
    {
        return;
    }

    fprintf(file, "%s", log_prefix);
    vfprintf(file, format, args);
    fprintf(file, "\n");

    fclose(file);
}

void Logger::output_level(int level, const char* format, va_list& args)
{
    assert(level >= 0 && level < 4);
    this->output(c_file_name_suffixes[level], c_log_prefixes[level], format, args);
}

void Logger::debug(const char* format, va_list& args)
{
    this->output_level(0, format, args);
}

void Logger::info(const char* format, va_list& args)
{
    this->output_level(1, format, args);
}

void Logger::warn(const char* format, va_list& args)
{
    this->output_level(2, format, args);
}

void Logger::error(const char* format, va_list& args)
{
    this->output_level(3, format, args);
}
