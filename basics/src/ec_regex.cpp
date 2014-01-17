#include "ec_regex.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

#include <regex.h>

using namespace std;

RegexImpl::RegexImpl()
    : _re(NULL)
{
}

RegexImpl::~RegexImpl()
{
    if (this->_re != NULL)
    {
        regfree(this->_re);
        delete this->_re;
    }
}

bool RegexImpl::Compile(const char* pczPattern)
{
    assert(pczPattern != NULL);
    if (this->_re != NULL)
    {
        regfree(this->_re);
        delete this->_re;
    }

    this->_re = new regex_t();
    if (regcomp(this->_re, pczPattern, REG_EXTENDED) != 0)
    {
        return false;
    }

    return true;
}

bool RegexImpl::IsMatch(const char* text)
{
    assert(text != NULL);
    vector<string> results;
    bool success = this->match(text, results, 1);
    if (success && results.size() == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

vector<string> RegexImpl::Match(const char* text, int match_count)
{
    assert(text != NULL);
    vector<string> results;

    this->match(text, results, match_count);
    return results;
}

bool RegexImpl::match(const char* text, vector<string>& results, int match_count)
{
    regmatch_t pm[match_count];
    char* begin = const_cast<char*>(text);
    int ret = regexec(this->_re, begin, match_count, pm, 0);
    if (ret != 0)
    {
        return false;
    }
    else if (pm[0].rm_so != -1)
    {
        for (int i = 0; i < match_count; ++i)
        {
            if (pm[i].rm_so != -1)
            {
                results.push_back(string(begin + pm[i].rm_so, pm[i].rm_eo - pm[i].rm_so));
            }
            else
            {
                break;
            }
        }

        begin += pm[0].rm_eo;
    }

    return true;
}

RegexFactorImpl::~RegexFactorImpl()
{
}

IRegex* RegexFactorImpl::Create()
{
    return new RegexImpl();
}

void RegexFactorImpl::Destroy(IRegex* pRegex)
{
    if (pRegex != NULL)
    {
        delete pRegex;
    }
}
