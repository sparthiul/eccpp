#ifndef _PYTHON_WRAPPER_H_
#define _PYTHON_WRAPPER_H_

#include <Python.h>

#include <vector>
#include <map>
#include <string>

//new reference: needs explicit dec_ref
//protected reference: auto dec_ref in clear()
class PythonWrapper
{
public:
    PythonWrapper() :
        _initialized(false)
    {
    }

    ~PythonWrapper();
    bool init();
    void clear(bool include_global = false); // dec all the protected references, and modules
    bool import_module(const char* module_name, bool import = true); // generates two protected references
    bool invoke(const char* module_name, const char* func_name, PyObject* param, PyObject*& result, bool protected_ref = true); // new or protected reference
    bool invoke(const char* module_name, const char* func_name, const std::string& text, PyObject*& result, bool protected_ref = true); // new or protected reference
    bool invoke(const char* module_name, const char* func_name, PyObject* param1, int param2, int param3, PyObject*& result, bool protected_ref); // new or protected reference
    bool invoke_object0(PyObject* obj, const char* func_name, PyObject*& result, bool protected_ref = true); // new or protected reference
    bool invoke_object1(PyObject* obj, const char* func_name, PyObject* param1, PyObject*& result, bool protected_ref = true); // new or protected reference
    bool invoke_object1(PyObject* obj, const char* func_name, const std::string& param1, PyObject*& result, bool protected_ref = true); // new or protected reference
    bool invoke_object2(PyObject* obj, const char* func_name, const char* param1, const char* param2, PyObject*& result, bool protected_ref = true); // new or protected reference
    bool invoke_object2(PyObject* obj, const char* func_name, int param1, PyObject* param2, PyObject*& result, bool protected_ref = true); // new or protected reference
    bool invoke_object2(PyObject* obj, const char* func_name, PyObject* param1, PyObject* param2, PyObject*& result, bool protected_ref = true); // new or protected reference
    PyObject* get_attr(PyObject* obj, const char* attr_name, bool protected_ref = true); // new or protected reference
    bool get_attr_string(PyObject* obj, const char* attr_name, std::string& result);
    bool set_attr(PyObject* obj, const char* attr_name, PyObject* value);
    bool set_attr_string(PyObject* obj, const char* attr_name, const std::string& value);
    bool get_list_items(PyObject* list, std::vector<PyObject*>& items); // borrowed reference
    PyObject* get_module_item(const char* module_name, const char* key); // borrowed reference
    bool to_string(PyObject* str_obj, std::string& result);
    bool is_instance(PyObject* object, PyObject* type);
    void dec_ref(PyObject* object);
    bool inc_ref(PyObject* object, bool protected_ref = true, bool global = false);
    PyObject* from_string(const std::string& str, bool protected_ref = true); // new or protected reference

private:
    bool check_object(PyObject* object, bool protected_ref = true, bool global = false);
    bool print_dict(PyObject* dict);
    bool handle_exception();

private:
    bool _initialized;
    std::map<const char*, PyObject*> _modules;
    std::map<PyObject*, int> _objects;
    std::map<PyObject*, int> _global_objects;
};

#endif
