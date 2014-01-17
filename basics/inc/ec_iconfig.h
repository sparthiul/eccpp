#ifndef _ICONFIG_H_
#define _ICONFIG_H_

#include <string>
#include <vector>

class IConfiguration
{
public:
    virtual bool HasKey(const std::string& section, const std::string& key) const = 0;
    virtual std::string GetValue(const std::string& section, const std::string& key) const = 0;
    virtual std::string GetValue(const std::string& section, const std::string& key, const std::string& default_value) const = 0;
    virtual int GetIntValue(const std::string& section, const std::string& key) const = 0;
    virtual int GetIntValue(const std::string& section, const std::string& key, int default_value) const = 0;
    virtual double GetDoubleValue(const std::string& section, const std::string& key) const = 0;
    virtual double GetDoubleValue(const std::string& section, const std::string& key, double default_value) const = 0;
    virtual bool GetBoolValue(const std::string& section, const std::string& key) const = 0;
    virtual bool GetBoolValue(const std::string& section, const std::string& key, bool default_value) const = 0;
    virtual std::string GetStringValue(const std::string& section, const std::string& key) const = 0;
    virtual bool GetStringList(const std::string& section, const std::string& key, std::vector<std::string>& list, const char* delimeter = "") const = 0;
    virtual const std::vector<std::string>& GetStringList(const std::string& section, const std::string& key, const char* delimeter = "") const = 0;
    virtual const std::vector<double>& GetDoubleList(const std::string& section, const std::string& key, const char* delimeter = "") const = 0;

    virtual ~IConfiguration()
    {
    }
};

#endif
