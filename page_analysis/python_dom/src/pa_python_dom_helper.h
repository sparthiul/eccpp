#ifndef _PYTHON_DOM_HELPER_H_
#define _PYTHON_DOM_HELPER_H_

#include "pa_python_wrapper.h"
#include <string>
#include <vector>

class PythonDomHelper
{
public:
    PythonDomHelper() :
        _initialized(false),
        _python(NULL),
        _html_element_type(NULL)
    {
    }

    bool init(PythonWrapper* python);
    PyObject* create_dom(const std::string& html); // protected reference
    PyObject* create_node(const std::string& tag, const std::string& text); // protected reference
    bool drop_node(PyObject* node, bool release_node);
    bool append_child(PyObject* node, PyObject* child);
    bool set_attribute(PyObject* node, const std::string& name, const std::string& value);
    bool remove_attribute(PyObject* node, const std::string& name);
    std::string get_attribute(PyObject* node, const std::string& name);
    bool get_attr_string(PyObject* node, const std::string& name, std::string& value);
    bool get_children(PyObject* node, std::vector<PyObject*>& children, bool tag_only);
    void release_node(PyObject* node);
    bool to_string(PyObject* node, std::string& output, bool debug = false);
    bool set_tag(PyObject* node, const std::string& new_tag);
    bool insert_child(PyObject* node, int position, PyObject* child);
    bool find_by_xpath(PyObject* node, const std::string& xpath, std::vector<PyObject*>& results);
    bool set_text(PyObject* node, std::string text);
    bool set_tail(PyObject* node, std::string tail);

    bool is_comment(PyObject* node);

private:
    bool _initialized;
    PythonWrapper* _python;
    PyObject* _html_element_type;
    PyObject* _comment_type;
};

#endif
