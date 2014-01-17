#ifndef _SERVER_PAGE_ANALYSIS_FACTORY_H_
#define _SERVER_PAGE_ANALYSIS_FACTORY_H_

#include "pa_page_analysis_factory.h"
#include <map>
#include <string>
#include <vector>

class PythonWrapper;
class PythonDomHelper;
class DomGlobalContext;
class DomTreeParser;
class ServerDomNodeVisitor;
class Logger;
class PageAnalysisFacade;
class DomNode;
class ListPageClassifier;
class IRegexFactor;

class ServerPageAnalysisFactory : public PageAnalysisFactory
{
public:
    static void get_default_config_files(std::map<std::string, std::string>& config_files);

    ServerPageAnalysisFactory();

    virtual ~ServerPageAnalysisFactory();

    bool init(const char* log_prefix, const std::map<std::string, std::string>& config_files);

    bool init(const char* log_prefix, const std::map<std::string, std::string>& config_files, const std::vector<std::string>& required_attributes, const std::string& url_folder = "");

    bool init(const std::string& url_folder = "");

    virtual void reset();

    virtual DomNodeKey load_dom(const std::string& url);
    virtual DomNodeKey load_dom_by_content(const std::string& url, const std::string& html_content);
    virtual DomNodeKey load_dom_by_file(const std::string& url, const std::string& file_name);

    DomTreeParser* parser() const
    {
        return this->_parser;
    }

    PythonDomHelper* helper() const
    {
        return this->_helper;
    }

    PythonWrapper* python() const
    {
        return this->_python;
    }

    bool get_list_page_classifier_result(const char* url, const char* file_name,
        bool& result, std::vector<double>& features, std::vector<int>& internal_features);

    ListPageClassifier* get_list_page_classifier();

    const std::vector<std::string>& folder_urls() const
    {
        return this->_folder_urls;
    }

    bool initialized() const
    {
        return this->_initialized;
    }

    virtual IConfigStore& create_configuration();
    virtual bool load_configuration(IConfigStore& configuration);

protected:
    virtual IDomNodeVisitor* create_dom_node_visitor();

    virtual IRegexFactor* create_regex_factor();

    virtual ILog* create_logger();

private:
    bool _initialized;
    PythonWrapper* _python;
    PythonDomHelper* _helper;
    DomGlobalContext* _global_context;
    DomTreeParser* _parser;
    std::string _log_file_prefix;
    std::map<std::string, std::string> _config_files;
    std::string _url_folder;
    std::vector<std::string> _folder_urls;
};

#endif
