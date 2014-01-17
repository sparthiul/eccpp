#include "ec_test_common.h"
#include "ec_iregex.h"
#include "ec_regex.h"

using namespace std;

TEST(RegexTest, main)
{
    IRegexFactor* factor = new RegexFactorImpl();
    IRegex* re = factor->Create();
   
    if (!re->Compile("([a-z]+ )+world"))
    {
        EXPECT_TRUE(false);
    }

    if (!re->IsMatch("hello world"))
    {
        EXPECT_TRUE(false);
    }

    vector<string> results = re->Match("+++ hello world++");
    EXPECT_TRUE(results.size() == 2);
    EXPECT_TRUE(results[0].compare("hello world") == 0);
    EXPECT_TRUE(results[1].compare("hello ") == 0);

    factor->Destroy(re);

    re = factor->Create();
    if (!re->Compile("www\\.qidian\\.com/BookReader/[0-9]*,[0-9]*\\.aspx"))
    {
        EXPECT_TRUE(false);
    }

    EXPECT_TRUE(re->IsMatch("http://www.qidian.com/BookReader/2404204,41632459.aspx"));
    EXPECT_TRUE(!re->IsMatch("http://www.qidian.com/BookReader/2404204.41632459.aspx"));

    delete factor;
}
