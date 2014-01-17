#ifndef _DOM_TREE_PARSER_H_
#define _DOM_TREE_PARSER_H_

class DomNode;
class DomGlobalContext;

#include <string>
#include <vector>
#include "pa_python_dom_helper.h"

class DomTreeParser
{
public:
    DomTreeParser(DomGlobalContext* global_context);
    DomNode* parse(const std::string& html, bool tag_only = true);
private:
    DomNode* build_dom_tree(DomNode* parent, PyObject* node, bool tag_only);

private:
    DomGlobalContext* _global_context;
};

#endif
