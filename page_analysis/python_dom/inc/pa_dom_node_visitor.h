#ifndef _DOM_NODE_VISITOR_H_
#define _DOM_NODE_VISITOR_H_

class DomNode;
typedef DomNode* DomNodeKey;

#include <string>
#include <vector>

class IDomNodeVisitor
{
public:
    virtual std::string get_text(DomNodeKey node) const = 0;

    virtual std::string get_tag(DomNodeKey node) const = 0;

    virtual bool is_html_element(DomNodeKey node) const = 0;
    virtual bool is_text_element(DomNodeKey node) const = 0;
    virtual std::string get_document_title(DomNodeKey node) const = 0;
    
    // returns "" if attribute doesn't exist
    virtual std::string get_attribute(DomNodeKey node, const std::string& name) const = 0;

    // returns false if failed
    virtual bool set_attribute(DomNodeKey node, const std::string& name, const std::string& value) const = 0;

    // returns false if failed
    virtual bool remove_attribute(DomNodeKey node, const std::string& name) const = 0;
    virtual bool has_attribute(DomNodeKey node, const std::string& name) const = 0;
    
    // returns NULL if it's root already
    virtual DomNodeKey get_parent(DomNodeKey node) const = 0;

    // returns NULL if first child is null
    virtual DomNodeKey get_first_child(DomNodeKey node) const = 0;

    // returns NULL if next sibling is null
    virtual DomNodeKey get_next_sibling(DomNodeKey node) const = 0;

    virtual int get_child_count(DomNodeKey node) const = 0;

    // drop node from the whole dom tree, if release_node if true, the node memory will released, otherwise it'll be kept for reuse.
    virtual bool drop_node(DomNodeKey node, bool release_node) const = 0;

    // appends child node as the new last child of node, returns false if failed
    virtual bool append_child(DomNodeKey node, DomNodeKey child) const = 0;

    // create a new dom node, returns NULL if failed
    // Note: node needs to be deleted if didn't appended to the dom tree
    virtual DomNodeKey create_node(DomNodeKey root, const std::string& tag, const std::string& text) const = 0;

    // change the node's tag into new_tag, returns false if failed
    virtual bool set_tag(DomNodeKey node, const std::string& new_tag) const = 0;

    // inserts the child node into the n-th position, position range is [0 ~ N], N is the current child count of node
    // if position == N, it's the same as append_child
    // Note: child should be not in the dom tree!
    virtual bool insert_child(DomNodeKey node, int position, DomNodeKey child) const = 0;

    // deep copy the whole sub dom tree, source sub dom tree not changed, returns NULL if failed
    virtual DomNodeKey deep_copy(DomNodeKey node) const = 0;

    // find dom nodes by xpath, returns false if failed
    // Notes: currently xpath just support to find nodes, not text or attributes.
    virtual bool find_by_xpath(DomNodeKey node, const std::string& xpath, std::vector<DomNodeKey>& results) const = 0;

    virtual ~IDomNodeVisitor()
    {
    }
};
#endif
