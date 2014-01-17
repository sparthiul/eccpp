#include "ec_utils.h"
#include "ec_ctype.h"
#include "ec_log.h"

#include <cstring>
#include <algorithm>
#include <functional>
#include <vector>
#include <fstream>
#include <iostream>
#include <cassert>
#include <streambuf>

using namespace std;

ParseResult urlparse(const string& url)
{
    ParseResult result;
    const string prot_end("://");
    string::const_iterator prot_i = search(url.begin(), url.end(), prot_end.begin(), prot_end.end());
    result.protocol.reserve(distance(url.begin(), prot_i));
    transform(url.begin(), prot_i, back_inserter(result.protocol), ptr_fun<int,int>(__tolower));
    if(prot_i == url.end())
        return result;

    advance(prot_i, prot_end.length());
    string::const_iterator path_i = find(prot_i, url.end(), '/');
    result.host.reserve(distance(prot_i, path_i));
    transform(prot_i, path_i, back_inserter(result.host), ptr_fun<int,int>(__tolower));

    string::const_iterator query_i = find(path_i, url.end(), '?');
    result.path.assign(path_i, query_i);
    if(query_i != url.end())
        ++query_i;
    result.query.assign(query_i, url.end());
    return result;
}

string urljoin(string base, string url)
{
    // 1. url contains protocol or host, return
    // 2. url start with /
    // 3. url path start without /
    // 3. url start with ../
    // 4. url just file name
    ParseResult result = urlparse(url);
    if (result.host.length() > 0)
    {
        return url;
    }
    result = urlparse(base);
    if (result.protocol.length() == 0 || result.host.length() == 0)
    {
        // base url invalid
        return url;
    }
    string abs = "/";
    string rel = "../";
    string path = result.path;
    size_t pos;
    if (path.rfind("/") != path.length() - 1)
    {
        pos = path.rfind("/");
        path = path.substr(0, pos + 1);
    }
    if (url.compare(0, abs.length(), abs) == 0)
    {
        return result.protocol + "://" + result.host + url;
    }
    else if (url.compare(0, rel.length(), rel) == 0)
    {
        while ((pos = url.find(rel)) == 0)
        {
            url = url.substr(rel.length(), url.length() - rel.length());
            pos = path.rfind("/");
            path  = path.substr(0, pos - 1);
        }
        return result.protocol + "://" + result.host + path + url;
    }
    else
    {
        return result.protocol + "://" + result.host + path + url;
    }
}

string get_basename(const string& file_path)
{
    size_t pos = file_path.find_last_of('/');
    if (pos != string::npos)
    {
        return file_path.substr(pos + 1);
    }
    else
    {
        return file_path;
    }
}

void split(const string& str, const char* delimeter, vector<string>& segments, bool need_strip)
{
    char* buffer = new char[str.length() + 1];
    strncpy(buffer, str.c_str(), str.length());
    buffer[str.length()] = '\0';
    char* ptr = buffer;
    char* saveptr;
    char* token;
    while ((token = strtok_r(ptr, delimeter, &saveptr)) != NULL)
    {
        string token_str;
        if (need_strip)
        {
            token_str = strip(string(token));
            if (token_str.length() > 0)
            {
                segments.push_back(token_str);
            }
        }
        else
        {
            token_str = string(token);
            segments.push_back(token_str);
        }

        ptr = saveptr;
    }

    delete[] buffer;
}

int match_list(const string& str, const vector<string>& string_list, int pattern)
{
    size_t len = str.length();
    for (int i = 0; i < static_cast<int>(string_list.size()); ++i)
    {
        switch (pattern)
        {
        case 0: 
            if (str.compare(0, string_list[i].length(), string_list[i]) == 0)
            {
                return i;
            }

            break;
        case 1:
            if (str.compare(string_list[i]) == 0)
            {
                return i;
            }

            break;
        case 2:
            if (str.find(string_list[i]) != string::npos)
            {
                return i;
            }

            break;
        case 3:
            if (str.length() >= string_list[i].length() && str.compare(len - string_list[i].length(), string_list[i].length(), string_list[i]) == 0)
            {
                return i;
            }

            break;
        case 4:
            if (string_list[i].find(string(str)) != string::npos)
            {
                return i;
            }

            break;
        default:
            return -1;
        }
    }

    return -1;
}

static const string g_spaces = std::string(" \r\t\n\x0c\x0d");

int count_without_spaces(const string& str)
{
    int count = 0;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if ((unsigned char)(str[i]) <= 0xFF)
        {
            if (g_spaces.find_first_of(str[i]) == std::string::npos)
            {
                count++;
            }
        }
        else
        {
            //Note: We didn't consider encoding of the str, and space chars such as 0xa0 and \u3000 can't be skipped
            count++;
        }
    }

    return count;
}

string strip(string ori)
{
    string s = ori;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(__isspace))));
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(__isspace))).base(), s.end());
    return s;
}

string replace(string ori, string sub, string rep)
{
    size_t pos;
    string str = ori;
    while ((pos = str.find(sub)) != string::npos)
    {
        str = str.replace(pos, sub.length(), rep);
    }
    return str;
}

bool write_file(const char* file_path, const string& file_content)
{
    ofstream file(file_path);
    if (file.is_open())
    {
        file << file_content;
        file.close();
        return true;
    }
    return false;
}

bool read_file(const char* file_name, stringstream& output)
{
    ifstream file(file_name);
    if (file.is_open())
    {
        file.seekg(0, ios::end);
        string content;
        content.reserve(file.tellg());
        file.seekg(0, ios::beg);
        content.assign(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
        output << content;

        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

bool read_lines(const char* file_name, vector<string>& lines)
{
    ifstream file(file_name);
    if (file.is_open())
    {
        string line;
        while (file.good())
        {
            getline(file, line);
            lines.push_back(line);
        }

        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

int match_count(const std::string& str, const std::string& substr)
{
    int count = 0;
    size_t from = 0;
    while (from < str.length())
    {
        size_t pos = str.find(substr, from);
        if (pos != string::npos)
        {
            count++;
            from = pos + substr.length();
        }
        else
        {
            break;
        }
    }

    return count;
}

int match_count(const std::string& str, const std::vector<std::string>& string_list)
{
    int count = 0;
    for (size_t i = 0; i < string_list.size(); ++i)
    {
        count += match_count(str, string_list[i]);
    }

    return count;
}

string to_lower(string in)
{
    transform(in.begin(), in.end(), in.begin(), __tolower);
    return in;
}

string to_upper(string in)
{
    transform(in.begin(), in.end(), in.begin(), __toupper);
    return in;
}

bool locate_range(const int* size_array, int count, int index, int& from, int& to)
{
    assert(size_array != NULL);
    assert(count >= 0);

    if (index < 0 || index >= count)
    {
        return false;
    }

    int start = 0;
    for (int i = 0; i < index; ++i)
    {
        start += size_array[i];
    }

    from = start;
    to = start + size_array[index];
    return true;
}

bool bytes_to_int(const char* str, int length, unsigned int& result)
{
    if (str == NULL || length <= 0 || length > 4)
    {
        return false;
    }

    result = 0;
    for (int i = 0; i < length; ++i)
    {
        result += (static_cast<unsigned int>(static_cast<unsigned char>(str[i]))) << (i * 8);
    }

    return true;
}

bool int_to_bytes(unsigned int data, char*& str, int& length)
{
    //hard code to 4 to make sure portability.
    str = new char[4];
    length = 4;

    for (int i = 0; i < length; ++i)
    {
        str[i] = data & 0xFF;
        data >>= 8;
    }

    return true;
}

bool int_to_bytes(unsigned int data, string& bytes)
{
    char* str;
    int length;
    if (!int_to_bytes(data, str, length))
    {
        return true;
    }

    bytes.append(str, length);
    delete[] str;
    return true;
}

int read_dlf_stream_section(istream& stream, string& section)
{
    char length_buffer[5];
    stream.read(length_buffer, 5);
    if (stream.eof())
    {
        return 0;
    }

    if (stream.fail())
    {
        log_error("stream failed");
        return -1;
    }

    unsigned int length;
    if (!bytes_to_int(length_buffer, 4, length))
    {
        log_error("length to int failed");
        return -1;
    }

    log_debug("read length %d", length);

    if (length_buffer[4] != '\0')
    {
        log_error("validation byte failed, actual%c, position%d", length_buffer[4], stream.tellg());
        char* test_buffer = new char[100];
        stream.read(test_buffer, 99);
        test_buffer[99] = '\0';
        log_error("%.*s", 100, test_buffer);
        delete[] test_buffer;
        return -1;
    }

    char* data_buffer = new char[length];

    stream.read(data_buffer, length);
    if (stream.fail())
    {
        log_error("read data failed");
        delete[] data_buffer;
        return -1;
    }

    section.append(data_buffer, length);

    delete[] data_buffer;
    return 1;
}

bool write_dlf_stream_section(ostream& stream, const string& section)
{
    if (section.length() > 0xFFFFFFFF)
    {
        return false;
    }

    char* length_buffer = NULL;
    int length;
    log_debug("write length %d, %d", section.length(), static_cast<unsigned int>(section.length()));
    if (!int_to_bytes(static_cast<unsigned int>(section.length()), length_buffer, length))
    {
        return false;
    }

    stream.write(length_buffer, length);
    delete[] length_buffer;
    if (stream.fail())
    {
        return false;
    }

    stream.put('\0');
    if (stream.fail())
    {
        return false;
    }

    stream.write(section.data(), section.length());
    if (stream.fail())
    {
        return false;
    }

    return true;
}

bool read_dlf_stream_sections(istream& stream, std::vector<std::string>& sections)
{
    while (stream.good())
    {
        string section;
        int status = read_dlf_stream_section(stream, section);
        if (status == -1)
        {
            return false;
        }
        else if (status == 0)
        {
            return true;
        }
        else
        {
            sections.push_back(section);
        }
    }

    return true;
}

bool write_dlf_stream_sections(ostream& stream, const std::vector<std::string>& sections)
{
    for (size_t i = 0; i < sections.size(); ++i)
    {
        if (!write_dlf_stream_section(stream, sections[i]))
        {
            return false;
        }
    }

    return true;
}

bool read_dlf_stream_sections(const string& file_name, std::vector<std::string>& sections)
{
    stringstream file_stream;
    if (!read_file(file_name.c_str(), file_stream))
    {
        return false;
    }

    if (!read_dlf_stream_sections(file_stream, sections))
    {
        return false;
    }

    return true;
}

bool write_dlf_stream_sections(const string& file_name, const std::vector<std::string>& sections)
{
    stringstream file_stream;
    if (!write_dlf_stream_sections(file_stream, sections))
    {
        return false;
    }

    if (!write_file(file_name.c_str(), file_stream.str()))
    {
        return false;
    }

    return true;
}

string remove_spaces(const string& ori)
{
    string result;
    for (size_t i = 0; i < ori.length(); ++i)
    {
        if (ori[i] != ' ')
        {
            result.push_back(ori[i]);
        }
    }

    return result;
}

bool misdigit(char c)
{
    return isdigit(c);
}

bool string_to_int(const char* str, int length, int& result)
{
    if (str == NULL || length <= 0)
    {
        return false;
    }

    int res = 0;
    bool sign = true;
    for (int i = 0; i < length; ++i)
    {
        if (i == 0 && (str[i] == '-' || str[i] == '+') && length > 1)
        {
            if (str[i] == '-')
            {
                sign = false;
            }
        }
        else if (str[i] <= '9' && str[i] >= '0')
        {
            res *= 10;
            res += str[i] - '0';
        }
        else
        {
            return false;
        }
    }

    if (!sign)
    {
        res *= -1;
    }

    result =  res;
    return true;
}

bool string_to_double(const char* str, int length, double& result)
{
    if (str == NULL || length <= 0)
    {
        return false;
    }

    double res = 0;
    bool sign = true;
    double minor_count = 0;
    for (int i = 0; i < length; ++i)
    {
        if (i == 0 && (str[i] == '-' || str[i] == '+') && length > 1)
        {
            if (str[i] == '-')
            {
                sign = false;
            }
        }
        else if (str[i] <= '9' && str[i] >= '0')
        {
            if (minor_count == 0)
            {
                res *= 10;
                res += str[i] - '0';
            }
            else
            {
                res += (str[i] - '0') / minor_count;
                minor_count *= 10;
            }
        }
        else if (str[i] == '.' && minor_count == 0)
        {
            minor_count = 10;
        }
        else
        {
            return false;
        }
    }

    if (!sign)
    {
        res *= -1;
    }

    result =  res;
    return true;
}

bool validate_args(int argc, char* argv[], const char* usage, int min_argc, int max_argc)
{
    assert(min_argc >= 0);
    assert(max_argc == -1 || max_argc == -2 || max_argc >= min_argc);
    assert(argv != NULL && usage != NULL);

    if (argc >= min_argc && ((max_argc >= 0 && argc <= max_argc) || (max_argc == -1 && argc == max_argc) || max_argc == -2))
    {
        return true;
    }

    cout << usage << endl;
    return false;
}

bool add_int_to_sections(unsigned int data, std::vector<std::string>& sections)
{
    std::string data_str;
    if (!int_to_bytes(data, data_str))
    {
        return false;
    }
    else
    {
        sections.push_back(data_str);
        return true;
    }
}

template<>
bool parse_string<int>(const char* str, int length, int& result)
{
    return string_to_int(str, length, result);
}

template<>
bool parse_string<double>(const char* str, int length, double& result)
{
    return string_to_double(str, length, result);
}
