#pragma once

#include <string>
#include <vector>

class IRegex
{
public:
    virtual ~IRegex() {}
    
    /// compile the regex pattern, return true if success
    virtual bool Compile(const char* pczPattern) = 0;
    
    /// return true if match, returns true if regex matched any substring of text.
    virtual bool IsMatch(const char* text) = 0;
    
    /** return the match result,
            res[0] is the whole matched string, 
            res[n] is the n-th matched sub string.
        if res.size() == 0, means not match.
    */
    virtual std::vector<std::string> Match(const char* text, int match_count = 1024) = 0;
};

class IRegexFactor
{
public:
    virtual ~IRegexFactor() {}
    
    virtual IRegex* Create() = 0;
    virtual void Destroy(IRegex* pRegex) = 0; 
};
