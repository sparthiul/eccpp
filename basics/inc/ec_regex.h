#ifndef _REGEX_IMPL_H_
#define _REGEX_IMPL_H_

#include "ec_iregex.h"

#include <string>
#include <vector>
#include <regex.h>

class RegexImpl : public IRegex
{
public:
    RegexImpl();

    ~RegexImpl();

    virtual bool Compile(const char* pczPattern);

    virtual bool IsMatch(const char* text);

    virtual std::vector<std::string> Match(const char* text, int match_count);

private:
    bool match(const char* text, std::vector<std::string>& results, int match_count);

private:
    regex_t* _re;
};

class RegexFactorImpl : public IRegexFactor
{
public:
    virtual ~RegexFactorImpl();

    virtual IRegex* Create();

    virtual void Destroy(IRegex* pRegex);
};

#endif
