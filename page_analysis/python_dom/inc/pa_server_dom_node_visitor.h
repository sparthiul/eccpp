#ifndef _SERVER_DOM_NODE_VISITOR_H_
#define _SERVER_DOM_NODE_VISITOR_H_

#include "pa_dom_node_visitor.h"

#include "pa_dom_node.h"

#include <string>

class ServerDomNodeVisitor : public IDomNodeVisitor
{
public:
    virtual std::string get_text(DomNodeKey node) const;
    virtual std::string get_tag(DomNodeKey node) const;
    virtual bool is_html_element(DomNodeKey node) const;
    virtual std::string get_attribute(DomNodeKey node, const std::string& name) const;
    virtual bool set_attribute(DomNodeKey node, const std::string& name, const std::string& value) const;
    virtual bool remove_attribute(DomNodeKey node, const std::string& name) const;
    virtual DomNodeKey get_parent(DomNodeKey node) const;
    virtual DomNodeKey get_first_child(DomNodeKey node) const;
    virtual DomNodeKey get_next_sibling(DomNodeKey node) const;
    virtual int get_child_count(DomNodeKey node) const;
    virtual bool drop_node(DomNodeKey node, bool release_node) const;
    virtual bool append_child(DomNodeKey node, DomNodeKey child) const;
    virtual DomNodeKey create_node(DomNodeKey root, const std::string& tag, const std::string& text) const;
    virtual bool set_tag(DomNodeKey node, const std::string& new_tag) const;
    virtual bool insert_child(DomNodeKey node, int position, DomNodeKey child) const;
    virtual DomNodeKey deep_copy(DomNodeKey node) const;
    virtual bool find_by_xpath(DomNodeKey node, const std::string& xpath, std::vector<DomNodeKey>& results) const;
    virtual bool is_text_element(DomNode* node) const;
    virtual std::string get_document_title(DomNode* node) const;
    virtual bool has_attribute(DomNode* node, const std::string& name) const;
};

#endif
