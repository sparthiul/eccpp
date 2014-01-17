#include "config.h"

#include "pa_dom_tree_parser.h"
#include "pa_dom_node.h"
#include "ec_utils.h"

#include <string>
#include <vector>
#include <iostream>
#include "pa_log.h"
#include <cassert>

using namespace std;

DomTreeParser::DomTreeParser(DomGlobalContext* global_context) :
    _global_context(global_context)

{
    assert(global_context != NULL);
}

DomNode* DomTreeParser::parse(const string& html, bool tag_only)
{
    PyObject* py_dom = this->_global_context->python()->create_dom(html);
    if (py_dom == NULL)
    {
        return NULL;
    }

    DomNode* dom = this->build_dom_tree(NULL, py_dom, tag_only);

    return dom;
}

DomNode* DomTreeParser::build_dom_tree(DomNode* parent, PyObject* node, bool tag_only)
{
    string tag;
    if (this->_global_context->python()->is_comment(node))
    {
        tag = "#COMMENT";
    }
    else if (!this->_global_context->python()->get_attr_string(node, "tag", tag))
    {
        log_error("can't get tag");
    }

    string text;
    this->_global_context->python()->get_attr_string(node, "text", text);

    DomNode* dom_node = new DomNode(to_upper(tag), "", this->_global_context, node);

    if (count_without_spaces(strip(text)) > 0)
    {
        dom_node->append_child_simple(new DomNode("#TEXT", text, this->_global_context, NULL));
    }

    for (int i = 0; i < this->_global_context->required_attributes().size(); ++i)
    {
        dom_node->add_attribute(this->_global_context->required_attributes()[i], this->_global_context->python()->get_attribute(node, this->_global_context->required_attributes()[i]));
    }

    vector<PyObject*> children;
    if (!this->_global_context->python()->get_children(node, children, tag_only))
    {
        log_error("get children failed");
    }
    else
    {
        for (int i = 0; i < children.size(); ++i)
        {
            DomNode* child_node = this->build_dom_tree(dom_node, children[i], tag_only);
            dom_node->append_child_simple(child_node);
            string inner_tail;
            this->_global_context->python()->get_attr_string(children[i], "tail", inner_tail);
            if (inner_tail.length() > 0)
            {
                dom_node->append_child_simple(new DomNode("#TEXT", inner_tail, this->_global_context, NULL));
            }
        }
    }

    return dom_node;
}
