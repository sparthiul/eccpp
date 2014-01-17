#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "ec_iconfig.h"

class Config : public IConfiguration
{
public:
    virtual ~Config();
    bool Init(const char* conf_path);
    bool Init_ByString(const std::string& config_string);
    virtual bool HasKey(const std::string& section, const std::string& key) const;
    virtual std::string GetValue(const std::string& section, const std::string& key) const;
    virtual std::string GetValue(const std::string& section, const std::string& key, const std::string& default_value) const;
    virtual int GetIntValue(const std::string& section, const std::string& key) const;
    virtual int GetIntValue(const std::string& section, const std::string& key, int default_value) const;
    virtual double GetDoubleValue(const std::string& section, const std::string& key) const;
    virtual double GetDoubleValue(const std::string& section, const std::string& key, double default_value) const;
    virtual bool GetBoolValue(const std::string& section, const std::string& key) const;
    virtual bool GetBoolValue(const std::string& section, const std::string& key, bool default_value) const;
    virtual std::string GetStringValue(const std::string& section, const std::string& key) const;
    virtual bool GetStringList(const std::string& section, const std::string& key, std::vector<std::string>& list, const char* delimeter = "") const;
    virtual const std::vector<std::string>& GetStringList(const std::string& section, const std::string& key, const char* delimeter = "") const;
    virtual const std::vector<double>& GetDoubleList(const std::string& section, const std::string& key, const char* delimeter = "") const;
    void Set(const std::string& section, const std::string& key, const std::string& value);
private:
    bool Init_ByStream(std::istream& stream);
    static std::string GenUniqueKey(const std::string& section, const std::string& key);

    bool mWrite;
    std::string mPath;
    std::map<std::string, std::map<std::string, std::string> > mConfig;

    mutable std::map<std::string, int> m_int_values;
    mutable std::map<std::string, double> m_double_values;
    mutable std::map<std::string, bool> m_bool_values;
    mutable std::map<std::string, std::string> m_string_values;
    mutable std::map<std::string, std::vector<std::string> > m_string_lists;
    mutable std::map<std::string, std::vector<double> > m_double_lists;
};

#endif
