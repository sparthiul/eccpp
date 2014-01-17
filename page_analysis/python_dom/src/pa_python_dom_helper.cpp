#include "config.h"

#include "pa_python_dom_helper.h"
#include "ec_log.h"

#include <cassert>
#include <iostream>

using namespace std;

static const char* c_main_module = "lxml.html";

bool PythonDomHelper::init(PythonWrapper* python)
{
    assert(!this->_initialized);
    assert(python != NULL);

    this->_python = python;

    if (!_python->import_module(c_main_module))
    {
        return false;
    }
    if (!_python->import_module("lxml.etree"))
    {
        return false;
    }

    this->_html_element_type = python->get_module_item(c_main_module, "HtmlElement");
    if (this->_html_element_type == NULL)
    {
        return false;
    }
    this->_comment_type = python->get_module_item("lxml.etree", "Comment");
    if (this->_comment_type == NULL)
    {
        return false;
    }

    this->_initialized = true;
    return true;
}

PyObject* PythonDomHelper::create_node(const std::string& tag, const std::string& text)
{
    if (tag.length() == 0)
    {
        return NULL;
    }

    PyObject* node = NULL;
    bool success = this->_python->invoke(c_main_module, "Element", tag, node, true);
    if (!success)
    {
        return NULL;
    }

    success = this->_python->set_attr_string(node, "text", text);
    if (!success)
    {
        log_error("set text failed");
        return NULL;
    }

    return node;
}

bool PythonDomHelper::drop_node(PyObject* node, bool release_node)
{
    assert(node != NULL);

    PyObject* result = NULL;
    bool success = this->_python->invoke_object0(node, "drop_tree", result, false);
    this->_python->dec_ref(result);
    if (release_node)
    {
        this->_python->dec_ref(node);
    }

    return success;
}

bool PythonDomHelper::append_child(PyObject* node, PyObject* child)
{
    assert(node != NULL);
    assert(child != NULL);

    PyObject* result = NULL;
    bool success = this->_python->invoke_object1(node, "append", child, result, false);
    this->_python->dec_ref(result);
    return success;
}

bool PythonDomHelper::set_attribute(PyObject* node, const std::string& name, const std::string& value)
{
    assert(node != NULL);

    PyObject* result = NULL;
    PyObject* n = this->_python->from_string(name);
    PyObject* v = this->_python->from_string(value);
    bool success = this->_python->invoke_object2(node, "set", n, v, result, false); //Note: take care of \0 string
    this->_python->dec_ref(result);
    this->_python->dec_ref(n);
    this->_python->dec_ref(v);
    return success;
}

bool PythonDomHelper::remove_attribute(PyObject* node, const std::string& name)
{
    assert(node != NULL);

    PyObject* attr = this->_python->get_attr(node, "attrib", false);
    if (attr == NULL)
    {
        return false;
    }

    PyObject* result;
    bool success = this->_python->invoke_object1(attr, "pop", name.c_str(), result, false); //Note: take care of \0 string
    this->_python->dec_ref(attr);
    this->_python->dec_ref(result);
    return success;
}

string PythonDomHelper::get_attribute(PyObject* node, const std::string& name)
{
    assert(node != NULL);

    PyObject* attr = this->_python->get_attr(node, "attrib", false);
    if (attr == NULL)
    {
        return "";
    }

    PyObject* result;
    if (!this->_python->invoke_object2(attr, "get", name.c_str(), "", result, false))
    {
        this->_python->dec_ref(attr);
        return "";
    }
    else
    {
        string text;
        if (!this->_python->to_string(result, text))
        {
            log_error("can't get attrib");
        }

        this->_python->dec_ref(attr);
        this->_python->dec_ref(result);
        return text;
    }
}

PyObject* PythonDomHelper::create_dom(const string& html)
{
    if (html.length() == 0)
    {
        return NULL;
    }

    PyObject* py_dom;
    if (!this->_python->invoke(c_main_module, "fromstring", html, py_dom, true))
    {
        return NULL;
    }
    else
    {
        return py_dom;
    }
}

bool PythonDomHelper::get_attr_string(PyObject* node, const string& name, string& value)
{
    assert(node != NULL);

    return this->_python->get_attr_string(node, name.c_str(), value);
}

bool PythonDomHelper::get_children(PyObject* node, vector<PyObject*>& children, bool tag_only)
{
    assert(node != NULL);

    PyObject* children_obj = NULL;
    // Note: below set to true may cause PyObject_GC_UnTrack segment fault if dec_ref is enabled
    if (!this->_python->invoke_object0(node, "getchildren", children_obj, true))
    {
        return false;
    }

    vector<PyObject*> all_children;
    if (!this->_python->get_list_items(children_obj, all_children))
    {
        return false;
    }

    // Note: list object's dec_ref will invoke item's dec_ref
    //this->_python->dec_ref(children_obj);

    for (int i = 0; i < all_children.size(); ++i)
    {
        if (!tag_only || this->_python->is_instance(all_children[i], this->_html_element_type)
                || is_comment(all_children[i]))
        {
            children.push_back(all_children[i]);
        }
    }

    return true;
}

void PythonDomHelper::release_node(PyObject* node)
{
    if (node != NULL)
    {
        this->_python->dec_ref(node);
    }
}

bool PythonDomHelper::to_string(PyObject* node, string& output, bool debug)
{
    assert(node != NULL);

    PyObject* output_obj = NULL;

    bool success;
    if (debug)
    {
        this->_python->import_module("lxml.etree", false);
        success = this->_python->invoke("lxml.etree", "tostring", node, output_obj, false);
    }
    else
    {
        success = this->_python->invoke(c_main_module, "tostring", node, false, true, output_obj, false);
    }

    if (!success)
    {
        return false;
    }

    success = this->_python->to_string(output_obj, output);
    this->_python->dec_ref(output_obj);
    return success;
}

bool PythonDomHelper::set_tag(PyObject* node, const string& new_tag)
{
    assert(node != NULL);

    return this->_python->set_attr_string(node, "tag", new_tag);
}

bool PythonDomHelper::insert_child(PyObject* node, int position, PyObject* child)
{
    assert(node != NULL);
    assert(child != NULL);

    PyObject* result = NULL;
    bool success = this->_python->invoke_object2(node, "insert", position, child, result, false);
    this->_python->dec_ref(result);
    return success;
}

bool PythonDomHelper::find_by_xpath(PyObject* node, const string& xpath, vector<PyObject*>& results)
{
    PyObject* result = NULL;
    bool success = this->_python->invoke_object1(node, "xpath", xpath, result, false);
    if (!success)
    {
        return false;
    }

    success = this->_python->get_list_items(result, results);

    this->_python->dec_ref(result);

    return success;
}

bool PythonDomHelper::set_text(PyObject* node, string text)
{
    return this->_python->set_attr_string(node, "text", text);
}

bool PythonDomHelper::set_tail(PyObject* node, string tail)
{
    return this->_python->set_attr_string(node, "tail", tail);
}

bool PythonDomHelper::is_comment(PyObject* node)
{
    return this->_python->get_attr(node, "tag") == this->_comment_type;
}
