#include "ec_test_common.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

void add_default_headers(string& html)
{
    string link = "<link rel='stylesheet' href='http://test1.dolphin-browser.com/yfhe/dolphinT.css' />";
    string script = "<script src='http://test1.dolphin-browser.com/yfhe/dolphinT.js' charset='utf-8' />";
    html = html + link + script;
}

bool load_hrs_file(const char* url_file, vector<pair<string, bool> >& urls)
{
    vector<string> lines;
    read_lines(url_file, lines);
    for (size_t i = 0; i < lines.size(); ++i)
    {
        if (lines[i].length() == 0)
        {
            continue;
        }

        size_t pos = lines[i].find_first_of(' ');
        if (pos == string::npos)
        {
            cout << "failed training.hrs at line " << i << endl;
            return false;
        }
        else
        {
            bool label;
            size_t next_pos = lines[i].find_first_of(' ', pos + 1);
            if (next_pos != string::npos)
            {
                label = lines[i].substr(pos + 1, next_pos - pos).compare("1") == 0 ? 1 : 0;
                if (lines[i].substr(next_pos + 1).compare("*") == 0)
                {
                    urls.push_back(pair<string, bool>("", false));
                }
                else
                {
                    urls.push_back(pair<string, bool>(lines[i].substr(0, pos), label));
                }
            }
            else
            {
                label = lines[i].substr(pos + 1).compare("1") == 0 ? 1 : 0;
                urls.push_back(pair<string, bool>(lines[i].substr(0, pos), label));
            }
        }
    }

    return true;
}
