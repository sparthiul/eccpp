#include "ec_log.h"
#include <cstdlib>
#include <cassert>

using namespace std;

#ifdef ANDROID

ILog* g_active_logger = NULL;

void set_active_logger(ILog* logger)
{
}

void config_logger(const char* log_prefix)
{
}

#include "android/android_log.h"

void log_debug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    dolphin_logv(LOG_PRI_DEBUG, format, args);
    va_end(args);
}

void log_info(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    dolphin_logv(LOG_PRI_INFO, format, args);
    va_end(args);
}

void log_warn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    dolphin_logv(LOG_PRI_WARN, format, args);
    va_end(args);
}

void log_error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    dolphin_logv(LOG_PRI_ERROR, format, args);
    va_end(args);
}
#else
#include "ec_logger.h"
void set_active_logger(ILog* logger)
{
}

static const char* g_log_prefix = "./log";

void config_logger(const char* log_prefix)
{
    assert(log_prefix != NULL);

    g_log_prefix = log_prefix;
}

void log_debug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Logger logger(g_log_prefix);
    logger.debug(format, args);
    va_end(args);
}

void log_info(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Logger logger(g_log_prefix);
    logger.info(format, args);
    va_end(args);
}

void log_warn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Logger logger(g_log_prefix);
    logger.warn(format, args);
    va_end(args);
}

void log_error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Logger logger(g_log_prefix);
    logger.error(format, args);
    va_end(args);
}
#endif
#if 0
#include <boost/thread/mutex.hpp>
static ILog* g_active_logger = NULL;

static boost::mutex g_logger_mutex;

void set_active_logger(ILog* logger)
{
    boost::mutex::scoped_lock lock(g_logger_mutex);
    g_active_logger = logger;
}

void log_debug(const char* format, ...)
{
    boost::mutex::scoped_lock lock(g_logger_mutex);
    if (g_active_logger == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    g_active_logger->debug(format, args);
    va_end(args);
}

void log_info(const char* format, ...)
{
    boost::mutex::scoped_lock lock(g_logger_mutex);
    if (g_active_logger == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    g_active_logger->info(format, args);
    va_end(args);
}

void log_warn(const char* format, ...)
{
    boost::mutex::scoped_lock lock(g_logger_mutex);
    if (g_active_logger == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    g_active_logger->warn(format, args);
    va_end(args);
}

void log_error(const char* format, ...)
{
    boost::mutex::scoped_lock lock(g_logger_mutex);
    if (g_active_logger == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    g_active_logger->error(format, args);
    va_end(args);
}
#endif
