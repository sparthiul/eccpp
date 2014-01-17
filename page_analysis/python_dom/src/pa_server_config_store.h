#ifndef _SERVER_CONFIG_STORE_H
#define _SERVER_CONFIG_STORE_H

#include "pa_dom_common.h"

class ServerConfigStore: public IConfigStore
{
public:
    static ServerConfigStore* GetInstance();
    ~ServerConfigStore();
    virtual void updateConfig(const std::string& key, const std::string& value);
    virtual std::string getConfig(const std::string& key) const;
    virtual std::map<std::string, std::string> getConfigMap()
    {
        return this->_config;
    }
private:
    ServerConfigStore(){};
    std::map<std::string, std::string> _config;
};

#endif
