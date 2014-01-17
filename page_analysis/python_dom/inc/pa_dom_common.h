#ifndef _DOM_COMMON_H_
#define _DOM_COMMON_H_

#include <map>
#include <string>

typedef void* DomContextHandle;

enum DocCategory
{
    DocCat_Generic,
    DocCat_News,
    DocCat_Novel,
    DocCat_Forum,
    DocCat_Wap,
    DocCat_Count,
};

enum DocType
{
    DocType_Undefined,
    DocType_Generic,
    DocType_TextContent,
    DocType_ListContent,
    DocType_Count,
};

enum TextInfoType
{
    TIT_PageTitle,
    TIT_PageContent,
    TIT_MenuLink,
    TIT_NextLink,
    TIT_PrevLink,
    TIT_Count,
};

class IConfigStore
{
public:
    virtual ~IConfigStore(){}
    virtual void updateConfig(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfig(const std::string& key) const = 0;
    // Get config map is for common test
    virtual std::map<std::string, std::string> getConfigMap() = 0;
};

#endif
