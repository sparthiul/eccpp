#include "pa_server_config_store.h"

using namespace std;

ServerConfigStore* ServerConfigStore::GetInstance()
{
    return (new ServerConfigStore());
}

ServerConfigStore::~ServerConfigStore()
{
    _config.clear();
}

void ServerConfigStore::updateConfig(const string& key, const string& value)
{
    _config[key] = value;
}

string ServerConfigStore::getConfig(const string& key) const
{
    map<string, string>::const_iterator iter = _config.find(key);
    if (iter == _config.end())
    {
        return "";
    }
    else
    {
        return iter->second;
    }
}
