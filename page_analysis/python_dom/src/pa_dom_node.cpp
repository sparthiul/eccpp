#include "config.h"

#include "pa_dom_node.h"

#include <iostream>
#include <assert.h>
#include <algorithm>
#include <cstring>
#include <cassert>

#include "ec_utils.h"
#include "ec_log.h"


using namespace std;

DomGlobalContext::DomGlobalContext(PythonDomHelper* python, const vector<string>& required_attributes) :
    _python(python)
{
    assert(python != NULL);
    for (int i = 0; i < required_attributes.size(); ++i)
    {
        this->_required_attributes.push_back(required_attributes[i]);
    }
}

DomGlobalContext::~DomGlobalContext()
{
}

DomNode* DomGlobalContext::get_node_by_index(PyObject* external_obj) const
{
    assert(external_obj != NULL);
    map<PyObject*, DomNode*>::const_iterator iter = this->_ei_index.find(external_obj);
    if (iter != this->_ei_index.end())
    {
        return iter->second;
    }
    else
    {
        return NULL;
    }
}

void DomGlobalContext::set_node_index(DomNode* node, PyObject* external_obj)
{
    assert(node != NULL);
    assert(external_obj != NULL);

    this->_ei_index[external_obj] = node;
}

void DomGlobalContext::remove_node_index(PyObject* external_obj)
{
    assert(external_obj != NULL);

    this->_ei_index.erase(external_obj);
}

PythonDomHelper* DomNode::python() const
{
    return this->_context->python();
}

DomNode::DomNode(const std::string& tag, const std::string& text, DomGlobalContext* context, PyObject* external_obj) :
    _tag(tag), _text(text), _context(context), _parent(NULL), _first_child(NULL), _last_child(NULL), _next_sibling(NULL), _external_obj(external_obj), _child_count(0)
{
    if (context != NULL && external_obj != NULL)
    {
        this->_context->set_node_index(this, external_obj);
    }
}

DomNode::~DomNode()
{
    for (DomNode* child = this->get_first_child(); child != NULL; )
    {
        DomNode* next_sibling = child->get_next_sibling();
        delete child;
        child = next_sibling;
    }

    if (this->_external_obj != NULL)
    {
        this->python()->release_node(this->_external_obj);
        this->_context->remove_node_index(this->_external_obj);
    }
}

void DomNode::append_child_simple(DomNode* child)
{
    child->_parent = this;
    if (this->_first_child == NULL)
    {
        this->_first_child = child;
    }

    if (this->_last_child != NULL)
    {
        this->_last_child->_next_sibling = child;
    }

    this->_last_child = child;

    this->_child_count++;
}

std::string DomNode::get_attribute(const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator iter = this->_attributes.find(name);
    if (iter != _attributes.end())
    {
        return iter->second.c_str();
    }
    else
    {
        return "";
    }
}

bool DomNode::drop_node(bool release_node)
{
    if (this->_parent != NULL)
    {
        this->_parent->_child_count--;
        DomNode* prev_node = NULL;
        for (DomNode* child = this->_parent->get_first_child(); child != NULL; child = child->get_next_sibling())
        {
            if (child == this)
            {
                if (prev_node != NULL)
                {
                    prev_node->_next_sibling = this->_next_sibling;
                }
                else
                {
                    this->_parent->_first_child = this->_next_sibling;
                }

                if (this->_next_sibling == NULL)
                {
                    this->_parent->_last_child = prev_node;
                }

                break;
            }
            else
            {
                prev_node = child;
            }
        }
        if (this->is_text_element())
        {
            if (prev_node == NULL)
            {
                this->python()->set_text(this->_parent->_external_obj, "");
            }
            else if (prev_node->_external_obj != NULL)
            {
                this->python()->set_tail(prev_node->_external_obj, "");
            }
        }
    }

    this->_parent = NULL;
    this->_next_sibling = NULL;

    if (this->_external_obj != NULL)
    {
        this->python()->drop_node(this->_external_obj, release_node);
    }

    if (release_node)
    {
        delete this;
    }

    return true;
}

bool DomNode::set_attribute(const std::string& name, const std::string& value)
{
    bool success = false;
    if (this->_external_obj != NULL && name.length() > 0)
    {
        success = this->python()->set_attribute(this->_external_obj, name, value);
        if (success)
        {
            this->_attributes[name] = value;
        }
    }

    return success;
}

bool DomNode::remove_attribute(const std::string& name)
{
    bool success = false;
    if (this->_external_obj != NULL && name.length() > 0)
    {
        success = this->python()->remove_attribute(this->_external_obj, name);
        if (success)
        {
            this->_attributes.erase(name);
        }
    }

    return success;
}

DomNode* DomNode::create_node(const string& tag, const string& text)
{
    PyObject* py_node = this->python()->create_node(to_lower(tag), text);
    if (py_node == NULL)
    {
        return NULL;
    }
    DomNode* node = new DomNode(tag, "", this->_context, py_node);
    if (text.length() > 0)
    {
        node->append_child_simple(new DomNode("#TEXT", text, this->_context, NULL));
    }
    return node;
}

bool DomNode::append_child(DomNode* node)
{
    assert(node != NULL);

    bool success = this->python()->append_child(this->_external_obj, node->_external_obj);
    if (success)
    {
        this->append_child_simple(node);
    }

    return success;
}

bool DomNode::set_tag(const string& new_tag)
{
    if (!this->python()->set_tag(this->_external_obj, new_tag))
    {
        return false;
    }

    this->_tag = new_tag;
    return true;
}

bool DomNode::insert_child(int position, DomNode* child)
{
    assert(child != NULL);

    if (position < 0 || position > this->_child_count)
    {
        return false;
    }
    int real_pos = position;
    int pos = position;
    for (DomNode* ch = this->get_first_child(); ch != NULL; ch = ch->get_next_sibling())
    {
        if (pos == 0)
        {
            break;
        }
        if (ch->is_text_element())
        {
            real_pos--;
        }
        pos--;
    }

    if (!this->python()->insert_child(this->_external_obj, real_pos, child->_external_obj))
    {
        return false;
    }

    int i = 0;
    DomNode* current = this->get_first_child();
    DomNode* prev = NULL;
    for (;i < position && current != NULL; ++i, current = current->get_next_sibling())
    {
        prev = current;
    }

    assert(i == position);
    child->_next_sibling = current;
    child->_parent = this;
    this->_child_count++;

    if (prev != NULL)
    {
        prev->_next_sibling = child;
    }
    else
    {
        this->_first_child = child;
    }

    if (current == NULL)
    {
        this->_last_child = child;
    }

    return true;
}

// Notes: this can be promoted to DomTreeVisitor
DomNode* DomNode::deep_copy()
{
    DomNode* clone_node = this->create_node(this->_tag, this->get_text());
    if (clone_node == NULL)
    {
        return NULL;
    }

    for (map<string, string>::const_iterator iter = this->_attributes.begin(); iter != this->_attributes.end(); ++iter)
    {
        if (!clone_node->set_attribute(iter->first, iter->second))
        {
            clone_node->drop_node(true);
            return NULL;
        }
    }

    for (DomNode* child = this->get_first_child(); child != NULL; child = child->get_next_sibling())
    {
        if (child->is_text_element())
        {
            continue;
        }
        DomNode* clone_child = child->deep_copy();
        if (clone_child == NULL)
        {
            clone_node->drop_node(true);
            return NULL;
        }
        else if (!clone_node->append_child(clone_child))
        {
            clone_node->drop_node(true);
            return NULL;
        }
    }

    return clone_node;
}

bool DomNode::find_by_xpath(const std::string& xpath, vector<DomNode*>& results) const
{
    results.clear();
    vector<PyObject*> external_results;
    bool success = this->python()->find_by_xpath(this->_external_obj, xpath, external_results);
    if (!success)
    {
        return false;
    }

    for (int i = 0; i < external_results.size(); ++i)
    {
        DomNode* node = this->_context->get_node_by_index(external_results[i]);
        if (node != NULL)
        {
            results.push_back(node);
        }
        else
        {
            log_error("external_obj can't be found in ei_index");
            results.clear();
            return false;
        }
    }

    return true;
}

const std::string DomNode::get_page_title() const
{
    const DomNode* prev = this;
    for (DomNode* parent = this->get_parent(); parent != NULL; parent = parent->get_parent())
    {
        prev = parent;
    }

    vector<DomNode*> results;
    if (!prev->find_by_xpath("./head/title", results))
    {
        return "";
    }
    else if (results.size() > 0)
    {
        return results[0]->get_text();
    }
    else
    {
        return "";
    }
}
