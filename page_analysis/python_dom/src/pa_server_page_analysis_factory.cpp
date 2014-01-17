#include "config.h"
#include "pa_server_page_analysis_factory.h"
#include "pa_page_analysis_facade.h"

#include "ec_utils.h"
#include "pa_server_dom_node_visitor.h"
#include "pa_dom_tree_parser.h"
#include "pa_python_wrapper.h"
#include "pa_page_analysis_facade.h"
#include "pa_page_analysis_facade_impl.h"
#include "ec_logger.h"
#include "ec_log.h"
#include "pa_dom_tree.h"
#include "pa_list_page_classifier.h"
#include "ec_regex_impl.h"
#include "pa_server_config_store.h"
#include "pa_algorithm_utils.h"

#include <cassert>
#include <algorithm>

using namespace std;

const std::string c_url_list_name = "url_list";
const std::string c_html_file_template = "%d.html";

ServerPageAnalysisFactory::ServerPageAnalysisFactory() :
    _initialized(false),
    _python(NULL),
    _helper(NULL),
    _global_context(NULL),
    _parser(NULL),
    _log_file_prefix(),
    _config_files()
{
}

ServerPageAnalysisFactory::~ServerPageAnalysisFactory()
{
    this->reset();
}

void ServerPageAnalysisFactory::reset()
{
    if (this->_python != NULL)
    {
        delete this->_python;
        this->_python = NULL;
    }

    if (this->_helper != NULL)
    {
        delete this->_helper;
        this->_helper = NULL;
    }

    if (this->_global_context != NULL)
    {
        delete this->_global_context;
        this->_global_context = NULL;
    }

    if (this->_parser != NULL)
    {
        delete this->_parser;
        this->_parser = NULL;
    }

    this->PageAnalysisFactory::reset();

    this->_initialized = false;
}

void ServerPageAnalysisFactory::get_default_config_files(std::map<std::string, std::string>& config_files)
{
    config_files["list_page_classifier.ini"] = "data/list_page_classifier.ini";
    config_files["list_page_classifier.svm"] = "data/list_page_classifier.svm";
    config_files["common_transcoder.ini"] = "data/common_transcoder.ini";
    config_files["detail_extractor.ini"] = "data/detail_extractor.ini";
    config_files["preprocess_transcoder.ini"] = "data/preprocess_transcoder.ini";
    config_files["white_list.ini"] = "data/white_list.ini";
    config_files["page_extractor_model.svm"] = "data/page_extractor_model.svm";
}

bool ServerPageAnalysisFactory::init(const char* log_prefix, const map<string, string>& config_files)
{
    vector<string> required_attributes;
    DomTreeVisitor::get_required_attributes(required_attributes);

    return this->init(log_prefix, config_files, required_attributes, "");
}

bool ServerPageAnalysisFactory::init(const string& url_folder)
{
    const char* log_prefix = "./page_analysis";

    map<string, string> config_files;
    get_default_config_files(config_files);

    vector<string> required_attributes;
    DomTreeVisitor::get_required_attributes(required_attributes);

    return this->init(log_prefix, config_files, required_attributes, url_folder);
}

bool ServerPageAnalysisFactory::init(const char* log_prefix, const map<string, string>& config_files, const vector<string>& required_attributes, const string& url_folder)
{
    assert(!this->_initialized);
    this->_log_file_prefix = log_prefix;
    this->_config_files = config_files;
    this->_url_folder = url_folder;

    if (!this->PageAnalysisFactory::init())
    {
        log_error("init page analysis factory failed");
        return false;
    }

    if (this->_url_folder.size() > 0)
    {
        string url_list_file = this->_url_folder;
        url_list_file.append(c_url_list_name);
        this->_folder_urls.clear();
        if (!read_lines(url_list_file.c_str(), this->_folder_urls))
        {
            log_error("read url list file failed");
            return false;
        }

        if (this->_folder_urls.size() > 0 && this->_folder_urls.back().length() == 0)
        {
            this->_folder_urls.erase(this->_folder_urls.end() - 1);
        }
    }

    this->_python = new PythonWrapper();
    if (!this->_python->init())
    {
        this->reset();
        return false;
    }

    this->_helper = new PythonDomHelper();
    if (!this->_helper->init(this->_python))
    {
        this->reset();
        return false;
    }

    this->_global_context = new DomGlobalContext(this->_helper, required_attributes);

    this->_parser = new DomTreeParser(this->_global_context);

    config_logger(this->_log_file_prefix.c_str());
    this->_initialized = true;
    return true;
}

DomNodeKey ServerPageAnalysisFactory::load_dom(const std::string& url)
{
    if (this->_folder_urls.size() == 0)
    {
        return NULL;
    }

    vector<string>::const_iterator iter = find(this->_folder_urls.begin(), this->_folder_urls.end(), url);
    if (iter == this->_folder_urls.end())
    {
        return NULL;
    }

    int index = iter - this->_folder_urls.begin();

    ostringstream file_name;
    file_name << this->_url_folder << index << ".html";
    return this->load_dom_by_file(url, file_name.str());
}

DomNodeKey ServerPageAnalysisFactory::load_dom_by_file(const string& url, const string& file_name)
{
    stringstream file_content_stream;
    read_file(file_name.c_str(), file_content_stream);
    string html_content = file_content_stream.str();
    
    return this->load_dom_by_content(url, html_content);
}

DomNodeKey ServerPageAnalysisFactory::load_dom_by_content(const std::string& url, const std::string& html_content)
{
    DomNode* dom = this->parser()->parse(html_content);
    if (dom == NULL)
    {
        log_error("load dom tree failed for url %s", url.c_str());
        return NULL;
    }

    return dom;
}

bool ServerPageAnalysisFactory::get_list_page_classifier_result(const char* url, const char* file_name,
    bool& result, vector<double>& features, vector<int>& internal_features)
{
    DomNode* dom = this->load_dom_by_file(url, file_name);
    if (dom == NULL)
    {
        return false;
    }
    
    DomContextHandle context = this->facade()->create_dom_context(dom, url);
    if (!this->facade()->preprocess(context))
    {
        dom->drop_node(true);
        log_error("preprocess failed for url %s", url);
        return false;
    }

    DocType dc_result = this->facade()->classify_page_type(context);
    result = dc_result == DocType_ListContent;

    DomTreeVisitor visitor(dom, *(this->node_visitor()));
    ListPageClassifier* classifier = this->get_list_page_classifier();
    classifier->extract_features(dom, visitor, url, features);
    classifier->traverse(dom, visitor, internal_features, url);

    this->facade()->free_dom_context(context);
    dom->drop_node(true);
    return true;
}

ListPageClassifier* ServerPageAnalysisFactory::get_list_page_classifier()
{
    ListPageClassifier* classifier = ((PageAnalysisFacadeImpl*)(this->facade()))->get_list_page_classifier();
    return classifier;
}

IDomNodeVisitor* ServerPageAnalysisFactory::create_dom_node_visitor()
{
    return new ServerDomNodeVisitor();
}

IConfigStore& ServerPageAnalysisFactory::create_configuration()
{
    return *ServerConfigStore::GetInstance();
}

bool ServerPageAnalysisFactory::load_configuration(IConfigStore& configuration)
{
    return ::load_configuration(this->_config_files, configuration);
}

IRegexFactor* ServerPageAnalysisFactory::create_regex_factor()
{
    return new RegexFactorImpl();
}

ILog* ServerPageAnalysisFactory::create_logger()
{
    return new Logger(this->_log_file_prefix.c_str());
}
