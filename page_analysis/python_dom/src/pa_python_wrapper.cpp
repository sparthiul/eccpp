#include "config.h"

#include "pa_python_wrapper.h"
#include "ec_log.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

PythonWrapper::~PythonWrapper()
{
    if (this->_initialized)
    {
        this->clear();
        //Py_Finalize();
    }
}

void PythonWrapper::clear(bool include_global)
{
    for (map<PyObject*, int>::iterator iter = this->_objects.begin(); iter != this->_objects.end(); ++iter)
    {
        for (int i = 0; i < iter->second; ++i)
        {
            Py_XDECREF(iter->first);
        }
    }

    this->_objects.clear();

    if (include_global)
    {
        for (map<PyObject*, int>::iterator iter = this->_global_objects.begin(); iter != this->_global_objects.end(); ++iter)
        {
            for (int i = 0; i < iter->second; ++i)
            {
                Py_XDECREF(iter->first);
            }
        }

        this->_global_objects.clear();
    }
}

bool PythonWrapper::init()
{
    assert(!this->_initialized);

    Py_Initialize();
    if (!Py_IsInitialized())
    {
        log_error("python init failed");
        return false;
    }

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");

    this->_initialized = true;
    return true;
}

bool PythonWrapper::check_object(PyObject* object, bool protected_ref, bool global)
{
    bool success = true;
    if (this->handle_exception())
    {
        success = false;
    }

    std::map<PyObject*, int>* objects = &(this->_objects);
    if (global)
    {
        objects = &(this->_global_objects);
    }

    if (object != NULL) // && (protected_ref || !success))
    {
        if (objects->find(object) != objects->end())
        {
            (*objects)[object]++;
        }
        else
        {
            (*objects)[object] = 1;
        }
    }

    if (object == NULL)
    {
        success = false;
    }

    return success;
}

bool PythonWrapper::import_module(const char* module_name, bool import)
{
    PyObject* module = NULL;
    if (import)
    {
        module = PyImport_ImportModule(module_name);
    }
    else
    {
        module = PyImport_AddModule(module_name);
    }

    if (!this->check_object(module, import, true))
    {
        return false;
    }

    PyObject* dict = PyModule_GetDict(module);
    if (!this->check_object(dict, import, true))
    {
        return false;
    }

    this->_modules[module_name] = dict;
    return true;
}

bool PythonWrapper::print_dict(PyObject* dict)
{
    if (!PyDict_Check(dict)) // no exception
    {
        log_error("invalid dict");
        return false;
    }

    PyObject* keys = PyDict_Keys(dict);
    if (!this->check_object(keys, false))
    {
        return false;
    }

    vector<PyObject*> children;
    if (this->get_list_items(keys, children))
    {
        for (int i = 0; i < children.size(); ++i)
        {
            string value;
            if (this->to_string(children[i], value))
            {
                log_debug(value.c_str());
            }
        }
    }

    this->dec_ref(keys);
    return true;
}

bool PythonWrapper::invoke(const char* module_name, const char* func_name, PyObject* param, PyObject*& result, bool protected_ref)
{
    //PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject* func = this->get_module_item(module_name, func_name);
    if (func == NULL)
    {
        return false;
    }

    result = PyObject_CallFunctionObjArgs(func, param, NULL);
    if (!this->check_object(result, protected_ref))
    {
        return false;
    }

    //PyGILState_Release(gstate);
    return true;
}

bool PythonWrapper::invoke(const char* module_name, const char* func_name, const string& text, PyObject*& result, bool protected_ref)
{
    PyObject* param = this->from_string(text, false);
    if (param == NULL || this->handle_exception())
    {
        return false;
    }

    bool success = this->invoke(module_name, func_name, param, result, protected_ref);
    this->dec_ref(param);
    return success;
}

bool PythonWrapper::invoke(const char* module_name, const char* func_name, PyObject* param1, int param2, int param3, PyObject*& result, bool protected_ref)
{
    PyObject* func = this->get_module_item(module_name, func_name);
    if (func == NULL)
    {
        return false;
    }

    result = PyObject_CallFunction(func, "Oii", param1, param2, param3);
    if (!this->check_object(result, protected_ref))
    {
        return false;
    }

    return true;
}

bool PythonWrapper::invoke_object0(PyObject* obj, const char* func_name, PyObject*& result, bool protected_ref)
{
    result = PyObject_CallMethod(obj, (char*)(func_name), NULL);
    return this->check_object(result, protected_ref);
}

bool PythonWrapper::invoke_object1(PyObject* obj, const char* func_name, PyObject* param1, PyObject*& result, bool protected_ref)
{
    PyObject* func = this->from_string(func_name, false);
    if (func == NULL)
    {
        return false;
    }

    result = PyObject_CallMethodObjArgs(obj, func, param1, NULL);
    this->dec_ref(func);
    return this->check_object(result, protected_ref);
}

bool PythonWrapper::invoke_object1(PyObject* obj, const char* func_name, const string& param1, PyObject*& result, bool protected_ref)
{
    PyObject* param1_obj = this->from_string(param1, false);

    bool success = this->invoke_object1(obj, func_name, param1_obj, result, protected_ref);
    this->dec_ref(param1_obj);
    return success;
}

bool PythonWrapper::invoke_object2(PyObject* obj, const char* func_name, PyObject* param1, PyObject* param2, PyObject*& result, bool protected_ref)
{
    PyObject* func = this->from_string(func_name);
    if (func == NULL)
    {
        return false;
    }

    result = PyObject_CallMethodObjArgs(obj, func, param1, param2, NULL);
    this->dec_ref(func);
    return this->check_object(result, protected_ref);
}

// TODO: use vararg list to refactor these invoke_object* functions
bool PythonWrapper::invoke_object2(PyObject* obj, const char* func_name, const char* param1, const char* param2, PyObject*& result, bool protected_ref)
{
    result = PyObject_CallMethod(obj, (char*)(func_name), (char*)"ss", param1, param2);
    return this->check_object(result, protected_ref);
}

bool PythonWrapper::invoke_object2(PyObject* obj, const char* func_name, int param1, PyObject* param2, PyObject*& result, bool protected_ref)
{
    result = PyObject_CallMethod(obj, (char*)(func_name), (char*)"iO", param1, param2);
    return this->check_object(result, protected_ref);
}

PyObject* PythonWrapper::get_attr(PyObject* obj, const char* attr_name, bool protected_ref)
{
    PyObject* attr = PyObject_GetAttrString(obj, attr_name);
    if (!this->check_object(attr, protected_ref))
    {
        return NULL;
    }
    else
    {
        return attr;
    }
}

bool PythonWrapper::set_attr(PyObject* obj, const char* attr_name, PyObject* value)
{
    bool success = PyObject_SetAttrString(obj, attr_name, value) != -1;
    if (this->handle_exception())
    {
        return false;
    }
    else
    {
        return success;
    }
}

bool PythonWrapper::set_attr_string(PyObject* obj, const char* attr_name, const string& value)
{
    PyObject* value_obj = this->from_string(value, false);
    if (value_obj == NULL)
    {
        return false;
    }

    bool success = this->set_attr(obj, attr_name, value_obj);
    this->dec_ref(value_obj);
    return success;
}

bool PythonWrapper::get_attr_string(PyObject* obj, const char* attr_name, string& result)
{
    PyObject* attr = this->get_attr(obj, attr_name, false);
    if (attr == NULL)
    {
        log_error("get_attr_string failed");
        return false;
    }

    bool success = this->to_string(attr, result);

    // Note: this will cause Py_XDECREF segment fault
    //this->dec_ref(attr);
    return success;
}

// Note: if list object is dec_ref, each item in the list will be dec_ref.
bool PythonWrapper::get_list_items(PyObject* list, vector<PyObject*>& items)
{
    if (!PyList_Check(list))
    {
        return false;
    }

    int count = PyList_GET_SIZE(list);
    if (this->handle_exception())
    {
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        PyObject* child = PyList_GET_ITEM(list, i); // it's borrowed reference
        if (child != NULL && !this->handle_exception())
        {
            items.push_back(child);
        }
    }

    return true;
}

PyObject* PythonWrapper::get_module_item(const char* module_name, const char* key)
{
    PyObject* result = PyDict_GetItemString(this->_modules[module_name], key);// it's borrowed reference
    if (this->handle_exception())
    {
        return NULL;
    }
    else
    {
        return result;
    }
}

bool PythonWrapper::to_string(PyObject* str_obj, string& result)
{
    //PyObject* decoded_obj = PyString_AsEncodedObject(str_obj, "utf-8", "ignore");
    assert(str_obj != NULL);

    if (str_obj == Py_None)
    {
        return true;
    }

    PyObject* encoded_obj = str_obj;
    bool encoded = false;
    if (PyUnicode_Check(str_obj)) // no exception
    {
        encoded_obj = PyUnicode_AsEncodedString(str_obj, "utf-8", "ignore");
        if (!this->check_object(encoded_obj), false)
        {
            return false;
        }

        encoded = true;
    }

    char* buffer = NULL;
    Py_ssize_t length = 0;
    int ret = PyString_AsStringAndSize(encoded_obj, &buffer, &length);
    if (ret == -1 || this->handle_exception())
    {
        if (encoded)
        {
            this->dec_ref(encoded_obj);
        }

        return false;
    }

    result.append(buffer, (size_t)length);
    if (encoded)
    {
        this->dec_ref(encoded_obj);
    }
    return true;
}

PyObject* PythonWrapper::from_string(const string& str, bool protected_ref)
{
    // Try decode with utf-8, if failed, try format as python string.
    PyObject* str_obj = PyUnicode_DecodeUTF8(str.data(), str.size(), "ignore");
    if (!this->check_object(str_obj, protected_ref))
    {
        str_obj = PyString_FromStringAndSize(str.data(), str.size());
        if (!this->check_object(str_obj, protected_ref))
        {
            return NULL;
        }
    }
    //str_obj = PyString_FromStringAndSize(str.data(), str.size());

    return str_obj;
}

bool PythonWrapper::is_instance(PyObject* object, PyObject* type)
{
    bool result = PyObject_IsInstance(object, type);
    if (this->handle_exception())
    {
        return false;
    }
    else
    {
        return result;
    }
}

bool PythonWrapper::handle_exception()
{
    if (PyErr_Occurred() != NULL)
    {
        //TODO: we can output exception info to logger via "traceback.format_exc()" or "PyErr_Fetch"
        PyErr_Clear();
        return true;
    }
    else
    {
        return false;
    }
}

void PythonWrapper::dec_ref(PyObject* object)
{
    if (object != NULL)
    {
        //PyGILState_STATE gstate = PyGILState_Ensure();
        // Note: there is segment fault or memory leak with dec_ref
        //Py_XDECREF(object);
        //PyGILState_Release(gstate);
    }
}

bool PythonWrapper::inc_ref(PyObject* object, bool protected_ref, bool global)
{
    Py_INCREF(object);
    return check_object(object, protected_ref, global);
}
