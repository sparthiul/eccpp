#ifndef _PAGE_ANALYSIS_ALGORITHMS_H_
#define _PAGE_ANALYSIS_ALGORITHMS_H_

#include "pa_dom_node_visitor.h"
#include "ec_log.h"
#include "pa_dom_common.h"

#include <string>
#include <map>

/*
configuration keys should be unique, currently used configuration keys:
list_page_classifier.ini
list_page_classifier.svm
common_transcoder.ini
*/

enum ClassifiyWay
{
    WHITE_LIST_FIRST,
    WHITE_LIST_ONLY,
    ALGORITHM_ONLY
};

class IRegexFactor;

class PageAnalysisFacade
{
public:
    // Returns a PageAnalysisFacade instance
    // Initiliaze all the global objects used inside algorithm, algorithm will not take responsibility on the release of these objects
    // Note: for andriod client side, logger here is ignored
    static PageAnalysisFacade* create(IDomNodeVisitor* visitor, const IConfigStore& configuration, IRegexFactor* _regexFactor, ILog* logger);

    static void destroy(PageAnalysisFacade* facade);
public:
    // Update configuration arguments when configuration changed, this method doesn't ensure thread safty
    virtual bool update_configuration(const std::string& key, const std::string& value) = 0;
    virtual bool update_configuration(const std::map<std::string, std::string>& configuration) = 0;

    // create data structure to store common features/data, shared by different algorithms
    virtual DomContextHandle create_dom_context(DomNodeKey dom, const char* url) = 0;

    // extract common features/data
    virtual bool preprocess(DomContextHandle context) = 0;

    // classify whether this page is list/details, etc.
    virtual DocType classify_page_type(DomContextHandle context, ClassifiyWay classifyWay = WHITE_LIST_FIRST) const = 0;

    // classify whether this page is news/forum, etc.
    virtual DocCategory classify_page_category(DomContextHandle context) const = 0;

    // returns main body of the dom tree
    virtual DomNodeKey extract_body(DomContextHandle context) const = 0;

    // extracts text structual info of the page, such as page title, specific links, etc.
    virtual std::string extract_text_info(DomContextHandle context, TextInfoType type) const = 0;

    // frees dom context
    virtual void free_dom_context(DomContextHandle context) = 0;

    // transcode
    virtual bool transcode(DomContextHandle context, bool paging=false) const = 0;

    // extract next page
    virtual std::string extract_next_page(DomContextHandle context) const = 0;

protected:
    virtual bool init(IDomNodeVisitor* visitor, IRegexFactor* _regexFactor, ILog* logger) = 0;

    PageAnalysisFacade();

    virtual ~PageAnalysisFacade();
};
#endif
