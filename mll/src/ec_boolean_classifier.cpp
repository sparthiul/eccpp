#include "ec_boolean_classifier.h"

#include <assert.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#include "ec_utils.h"
#include "ec_log.h"

using namespace std;

const char* c_comparer_ops[] = {"==", "!=", "<", "<=", ">", ">="};

static bool s_equal_to(double left, double right)
{
    return left == right;
}

static bool s_not_equal_to(double left, double right)
{
    return left != right;
}

static bool s_less(double left, double right)
{
    return left < right;
}

static bool s_less_equal(double left, double right)
{
    return left <= right;
}

static bool s_greater(double left, double right)
{
    return left > right;
}

static bool s_greater_equal(double left, double right)
{
    return left >= right;
}

bool (*c_comparers[])(double, double) = {s_equal_to, s_not_equal_to, s_less, s_less_equal, s_greater, s_greater_equal};

bool BooleanClassifier::init(const char* expression_str, const vector<string>& feature_names)
{
    assert(expression_str != NULL);
    vector<string> comparer_ops(c_comparer_ops, c_comparer_ops + sizeof(c_comparer_ops) / sizeof(const char*));

    vector<string> items;
    split(expression_str, " ", items, false);
    bool needs_op = false;
    int current_group_id = 0;
    bool current_not = false;
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (needs_op)
        {
            if (items[i].compare("&&") == 0)
            {
                needs_op = false;
            }
            else if (items[i].compare("||") == 0)
            {
                ++current_group_id;
                needs_op = false;
            }
            else
            {
                log_error("need and/or but met %s", items[i].c_str());
                return false;
            }
        }
        else
        {
            if (items[i].compare("!") == 0)
            {
                if (current_not)
                {
                    log_error("duplicate not is not allowed");
                    return false;
                }
                else
                {
                    current_not = true;
                }
            }
            else
            {
                int feature_id = match_list(items[i].c_str(), feature_names, 1);
                if (feature_id >= 0)
                {
                    if (i + 2 >= items.size())
                    {
                        log_error("no comparer op found, %s", items[i].c_str());
                        return false;
                    }

                    int comparer_op_id = match_list(items[i + 1].c_str(), comparer_ops, 1);
                    if (comparer_op_id < 0)
                    {
                        log_error("invalid comparer op, %s", items[i].c_str());
                        return false;
                    }

                    double right_value = atof(items[i + 2].c_str());

                    this->_expression.push_back(Atom(feature_id, feature_names[feature_id], current_not, comparer_op_id, right_value, current_group_id));
                    current_not = false;
                    needs_op = true;
                    i += 2;
                }
                else
                {
                    log_error("need not or feature name but met, %s", items[i].c_str());
                    return false;
                }
            }
        }
    }

    if (!needs_op)
    {
        log_error("ends with op");
        return false;
    }

    this->_initialized = true;
    return true;
}

bool BooleanClassifier::classify(const map<int, double>& features) const
{
    int current_group_id = 0;
    bool current_result = false;
    for (size_t i = 0; i < this->_expression.size(); ++i)
    {
        const Atom& atom = this->_expression[i];
        const map<int, double>::const_iterator iter = features.find(atom.feature_id);
        assert(iter != features.end());
        double feature_value = iter->second;

        assert(atom.comparer_op_id < static_cast<int>(sizeof(c_comparers) / sizeof(c_comparers[0])));

        if (current_result && atom.group_id > current_group_id)
        {
            return true;//return if one and group returns true
        }
        else if (atom.group_id < current_group_id)
        {
            continue; // skip if the and group is false
        }

        bool (*comparer)(double, double) = c_comparers[atom.comparer_op_id];
        bool result = comparer(feature_value, atom.right_value);
        if (atom.with_not)
        {
            result = !result;
        }

        if (!result)
        {
            ++current_group_id;
        }

        current_result = result;
    }

    return current_result;
}

bool BooleanClassifier::classify(const map<string, double>& features) const
{
    map<int, double> new_features;
    for (size_t i=0; i < this->_expression.size(); i++)
    {
        const Atom& atom = this->_expression[i];
        // assume feature name must exists in features.
        const map<string, double>::const_iterator iter = features.find(atom.feature_name);
        assert(iter != features.end());
        new_features.insert(map<int, double>::value_type(atom.feature_id, iter->second));
    }
    return this->classify(new_features);
}

std::vector<string> BooleanClassifier::get_feature_names()
{
    vector<string> names;
    for (size_t i=0; i < this->_expression.size(); i++)
    {
        const Atom& atom = this->_expression[i];
        names.push_back(atom.feature_name);
    }
    return names;
}

