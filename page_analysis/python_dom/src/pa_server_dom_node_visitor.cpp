#include "config.h"

#include "pa_server_dom_node_visitor.h"

#include <cassert>

using namespace std;

std::string ServerDomNodeVisitor::get_text(DomNodeKey node) const
{
    assert(node != NULL);
    return node->get_text();
}

std::string ServerDomNodeVisitor::get_tag(DomNodeKey node) const
{
    assert(node != NULL);
    return node->get_tag();
}

bool ServerDomNodeVisitor::is_html_element(DomNodeKey node) const
{
    assert(node != NULL);
    //return true;
    return get_tag(node).compare("#COMMENT") != 0;
}

std::string ServerDomNodeVisitor::get_attribute(DomNodeKey node, const std::string& name) const
{
    assert(node != NULL);
    return node->get_attribute(name);
}

bool ServerDomNodeVisitor::set_attribute(DomNodeKey node, const std::string& name, const std::string& value) const
{
    assert(node != NULL);
    return node->set_attribute(name, value);
}

bool ServerDomNodeVisitor::remove_attribute(DomNodeKey node, const std::string& name) const
{
    assert(node != NULL);
    return node->remove_attribute(name);
}

DomNodeKey ServerDomNodeVisitor::get_parent(DomNodeKey node) const
{
    assert(node != NULL);
    return node->get_parent();
}

DomNodeKey ServerDomNodeVisitor::get_first_child(DomNodeKey node) const
{
    assert(node != NULL);
    return node->get_first_child();
}

DomNodeKey ServerDomNodeVisitor::get_next_sibling(DomNodeKey node) const
{
    assert(node != NULL);
    return node->get_next_sibling();
}

int ServerDomNodeVisitor::get_child_count(DomNodeKey node) const
{
    assert(node != NULL);
    return node->get_child_count();
}

bool ServerDomNodeVisitor::drop_node(DomNodeKey node, bool release_node) const
{
    assert(node != NULL);
    return node->drop_node(release_node);
}

bool ServerDomNodeVisitor::append_child(DomNodeKey node, DomNodeKey child) const
{
    assert(node != NULL);
    assert(!node->is_text_element());
    return node->append_child(child);
}

// Note: node needs to be deleted if didn't appended to the dom tree
DomNodeKey ServerDomNodeVisitor::create_node(DomNodeKey root, const std::string& tag, const std::string& text) const
{
    assert(root != NULL);
    return root->create_node(tag, text);
}

bool ServerDomNodeVisitor::set_tag(DomNodeKey node, const std::string& new_tag) const
{
    assert(node != NULL);
    assert(!node->is_text_element());
    return node->set_tag(new_tag);
}

bool ServerDomNodeVisitor::insert_child(DomNodeKey node, int position, DomNodeKey child) const
{
    assert(node != NULL);
    assert(!node->is_text_element());
    return node->insert_child(position, child);
}

DomNodeKey ServerDomNodeVisitor::deep_copy(DomNodeKey node) const
{
    assert(node != NULL);
    return node->deep_copy();
}

bool ServerDomNodeVisitor::find_by_xpath(DomNodeKey node, const std::string& xpath, std::vector<DomNodeKey>& results) const
{
    assert(node != NULL);
    assert(!node->is_text_element());
    return node->find_by_xpath(xpath, results);
}
bool ServerDomNodeVisitor::is_text_element(DomNode* node) const
{
    assert(node != NULL);
    return node->is_text_element();
}

std::string ServerDomNodeVisitor::get_document_title(DomNode* node) const
{
    assert(node != NULL);
    return node->get_page_title();
}

bool ServerDomNodeVisitor::has_attribute(DomNode* node, const std::string& name) const
{
    assert(node != NULL);
    return node->has_attribute(name);
}
