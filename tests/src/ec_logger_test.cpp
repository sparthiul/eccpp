#include "ec_test_common.h"
#include "ec_log.h"
#include "ec_logger.h"
#include <cstdio>
#include <string>
#include <vector>


using namespace std;

TEST(LoggerTest, main)
{
    Logger logger("./page_analysis");
    set_active_logger(&logger);

    log_info("this is first log");
    log_error("My name is %s, age is %d", "tom", 10);
    log_warn("Hey");
    log_error("hello");
}
