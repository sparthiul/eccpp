#ifndef _EC_UTILS_H_
#define _EC_UTILS_H_

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cassert>
#include <iostream>
#include <istream>
#include <ostream>
#include <limits>
#include <cmath>

bool validate_args(int argc, char* argv[], const char* usage, int min_argc, int max_argc = -1);

struct ParseResult
{
public:
    ParseResult()
    {
    }

    ParseResult(const char* protocol, const char* host, const char* path, const char* query) :
        protocol(protocol), host(host), path(path), query(query)
    {
    }

    std::string protocol, host, path, query;
};

ParseResult urlparse(const std::string& url);
std::string urljoin(std::string base, std::string url);
std::string get_basename(const std::string& file_path);
void split(const std::string& str, const char* delimeter, std::vector<std::string>& segments, bool need_strip=true);

// pattern: 0: str startswith any
// pattern: 1: str full matches any
// pattern: 2: str contains any
// pattern: 3: str endswith any
// pattern: 4: string_list contains str
int match_list(const std::string& str, const std::vector<std::string>& string_list, int pattern = 0);

int match_count(const std::string& str, const std::string& substr);
int match_count(const std::string& str, const std::vector<std::string>& string_list);

int count_without_spaces(const std::string& str);
std::string strip(std::string ori);
std::string remove_spaces(const std::string& ori);
std::string replace(std::string ori, std::string sub, std::string rep);

bool write_file(const char* file_path, const std::string& file_content);
bool read_file(const char* file_name, std::stringstream& output);
bool read_lines(const char* file_name, std::vector<std::string>& lines);
std::string to_lower(std::string in);
std::string to_upper(std::string in);

// at most 4 bytes, small endian
bool bytes_to_int(const char* str, int length, unsigned int& result);
bool int_to_bytes(unsigned int data, char*& str, int& length);
bool int_to_bytes(unsigned int data, std::string& bytes);
bool add_int_to_sections(unsigned int data, std::vector<std::string>& sections);

// valid int value: [+|-]?[0-9]+
bool string_to_int(const char* str, int length, int& result);
//valid value: [+|-]?[0-9]*(.[0-9]*)?
bool string_to_double(const char* str, int length, double& result);

template<typename T>
bool parse_string(const char* str, int length, T& result)
{
    assert(false);//can't be called directly
    return false;
}

template<>
bool parse_string<int>(const char* str, int length, int& result);

template<>
bool parse_string<double>(const char* str, int length, double& result);

template<typename T>
std::string to_string(T value)
{
    std::stringstream stream;
    stream << value;
    return stream.str();
}

bool locate_range(const int* size_array, int count, int index, int& from, int& to);

// dlf file format: length(4 bytes, small endian), \0, data section with length, length, \0, data, ...
// Note: this file format can be compressed
int read_dlf_stream_section(std::istream& stream, std::string& section);
bool read_dlf_stream_sections(std::istream& stream, std::vector<std::string>& sections);
bool read_dlf_stream_sections(const std::string& file_name, std::vector<std::string>& sections);

bool write_dlf_stream_section(std::ostream& stream, const std::string& section);
bool write_dlf_stream_sections(std::ostream& stream, const std::vector<std::string>& sections);
bool write_dlf_stream_sections(const std::string& file_name, const std::vector<std::string>& sections);

template <class U, class V>
void map_to_stream(const std::map<U, V>& map_obj, std::ostream& output_stream)
{
    for (typename std::map<U, V>::const_iterator iter = map_obj.begin(); iter != map_obj.end(); ++iter)
    {
        output_stream << iter->first << ":" << iter->second << std::endl;
    }
}
template <class T>
void vector_to_stream(const std::vector<T>& vector_obj, std::ostream& output_stream)
{
    for (size_t i = 0; i < vector_obj.size(); ++i)
    {
        output_stream << vector_obj[i] << " ";
    }

    output_stream << std::endl;
}

bool misdigit(char c);

//just support int, double now
template<typename T>
int read_stream_matrix(std::istream& stream, int n, const char* delimeter, std::vector<std::vector<T> >& matrix)
{
    assert(n > 0);
    std::string line;
    while (stream.good())
    {
        std::vector<T> line;
        int res = read_stream_line(stream, n, delimeter, line);
        if (res == -1 || res == 0)
        {
            return res;
        }

        matrix.push_back(line);
    }

    return 1;
}

template<typename T>
int read_stream_line(std::istream& stream, int n, const char* delimeter, std::vector<T>& line)
{
    std::string raw_line;
    std::getline(stream, raw_line);
    if (raw_line.length() == 0 && stream.eof())
    {
        return 0;
    }

    std::vector<std::string> columns;
    split(raw_line, delimeter, columns);
    if (n >= 0 && columns.size() != static_cast<size_t>(n))
    {
        return -1;
    }

    line.assign(columns.size(), T());
    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (!parse_string<T>(columns[i].data(), columns[i].length(), line[i]))
        {
            return -1;
        }
    }

    return 1;
}

template <typename T>
bool greater_than_zero(T t)
{
    return t > 0;
}

template <typename T>
bool less_equal_to_zero(T t)
{
    return t <= 0;
}

inline bool double_equal(double first, double second)
{
    return std::fabs(first - second) < std::numeric_limits<double>::epsilon();
}

#define SUCCESS(func, ret_code) if (!(func)){return ret_code;}

#endif
