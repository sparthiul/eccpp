#ifndef _TEST_COMMON_H_
#define _TEST_COMMON_H_

#include "gtest/gtest.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "ec_utils.h"

using namespace std;

template <class T>
void compare_vector(const vector<T>& first, const vector<T>& second)
{
    EXPECT_TRUE(first == second);
}

template <class T>
void print_vector(const char* prefix, const vector<T>& v)
{
    cout << prefix << " ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        cout << v[i] << " ";
    }
}

void add_default_headers(string& html);

bool load_hrs_file(const char* url_file, vector<pair<string, bool> >& urls);

#endif
