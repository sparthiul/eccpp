#ifndef _PA_PAGE_ANALYSIS_FACTORY_H_
#define _PA_PAGE_ANALYSIS_FACTORY_H_

#include "pa_dom_node_visitor.h"
#include "pa_page_analysis_facade.h"

#include <map>
#include <string>

class PageAnalysisFactory
{
public:
    PageAnalysisFactory();
    virtual ~PageAnalysisFactory();

    bool init();

    virtual void reset();

    PageAnalysisFacade* facade() const
    {
        return this->_facade;
    }

    IDomNodeVisitor* node_visitor() const
    {
        return this->_node_visitor;
    }

    IRegexFactor* regex_factor() const
    {
        return this->_regex_factor;
    }

    ILog* logger() const
    {
        return this->_logger;
    }

    // loads dom tree by url, returns NULL if failed, needs caller to release.
    virtual DomNodeKey load_dom(const std::string& url) = 0;
    virtual DomNodeKey load_dom_by_content(const std::string& url, const std::string& html_content) = 0;
    virtual DomNodeKey load_dom_by_file(const std::string& url, const std::string& file_name) = 0;

protected:
    friend class CommonTestSuite;

    // returns NULL if failed, needs caller to release
    virtual IDomNodeVisitor* create_dom_node_visitor() = 0;

    // returns config store
    virtual IConfigStore& create_configuration() = 0;

    // returns false if failed
    virtual bool load_configuration(IConfigStore& configuration) = 0;

    // returns NULL if failed, needs caller to release
    virtual IRegexFactor* create_regex_factor() = 0;

    // returns NULL if failed, needs caller to release
    virtual ILog* create_logger() = 0;

private:
    bool _initialized;
    PageAnalysisFacade* _facade;
    IDomNodeVisitor* _node_visitor;
    IRegexFactor* _regex_factor;
    ILog* _logger;
};

#endif
