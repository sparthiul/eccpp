#ifndef _DOM_NODE_H_
#define _DOM_NODE_H_

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <cstdio>

#include "pa_python_wrapper.h"
#include "pa_python_dom_helper.h"

class DomGlobalContext;

class DomNode
{
public:
    DomNode(const std::string& tag, const std::string& text, DomGlobalContext* context = NULL, PyObject* external_obj = NULL);
    ~DomNode();

    // below APIs are used for dom tree building from python lxml object

    void append_child_simple(DomNode* child);

    void append_text(const std::string& text)
    {
        this->_text.append(text);
    }

    void add_attribute(const std::string& name, const std::string& value)
    {
        if (value.length() > 0)
        {
            this->_attributes[name] = value;
        }
    }

    // below readonly APIs are used for dom node visitor
    std::string get_text() const
    {
        if (is_text_element())
        {
            return this->_text;
        }
        else
        {
            std::string text;
            for (DomNode* child = get_first_child(); child != NULL; child = child->get_next_sibling())
            {
                if (child->is_text_element())
                {
                    text += child->get_text();
                }
            }
            return text;
        }
    }

    std::string get_tag() const
    {
        return this->_tag;
    }

    std::string get_attribute(const std::string& name) const;

    DomNode* get_parent() const
    {
        return this->_parent;
    }

    DomNode* get_first_child() const
    {
        return this->_first_child;
    }

    DomNode* get_next_sibling() const
    {
        return this->_next_sibling;
    }

    int get_child_count() const
    {
        return this->_child_count;
    }

    // below writable APIs are used for dom node visitor, will change internal python object
    bool set_attribute(const std::string& name, const std::string& value);

    bool remove_attribute(const std::string& name);

    bool drop_node(bool release_node);

    bool append_child(DomNode* node);

    DomNode* create_node(const std::string& tag, const std::string& text);

    bool set_tag(const std::string& new_tag);

    bool insert_child(int position, DomNode* child);

    DomNode* deep_copy();

    bool find_by_xpath(const std::string& xpath, std::vector<DomNode*>& results) const;

    bool has_attribute(const std::string& name) const
    {
        return this->_attributes.find(name) != this->_attributes.end();
    }

    bool is_text_element() const
    {
        return this->_tag == "#TEXT";
    }

    // can be prmoted to dom tree
    const std::string get_page_title() const;

    // below APIs are used for unit test

    PyObject* get_external_obj() const
    {
        return this->_external_obj;
    }

protected:
    PythonDomHelper* python() const;

protected:
    std::string _tag;
    std::string _text;
    DomGlobalContext* _context;
    int _child_count;
    PyObject* _external_obj;
    DomNode* _parent;
    DomNode* _first_child;
    DomNode* _last_child;
    DomNode* _next_sibling;
    std::map<std::string, std::string> _attributes;
};

class DomGlobalContext
{
public:
    DomGlobalContext(PythonDomHelper* python, const std::vector<std::string>& required_attributes);
    ~DomGlobalContext();

    DomNode* get_node_by_index(PyObject* external_obj) const;
    void set_node_index(DomNode* node, PyObject* external_obj);
    void remove_node_index(PyObject* external_obj);
    PythonDomHelper* python() const
    {
        return this->_python;
    }

    const std::vector<std::string>& required_attributes() const
    {
        return this->_required_attributes;
    }

private:
    std::map<PyObject*, DomNode*> _ei_index;
    PythonDomHelper* _python;
    std::vector<std::string> _required_attributes;
};

#endif
