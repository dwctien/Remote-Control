#ifndef _HTML_GENERATOR_H_
#define _HTML_GENERATOR_H_

#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

typedef unsigned char BYTE;
typedef pair<string, vector<BYTE>> Response;

struct DataFrame {
    string type; // "group" or "single"
    vector<string> columns; // Column names
    vector<vector<string>> data; // Row data
};

string html_table(const DataFrame& df, const string& note = "", const string& format = "center");

string html_msg(const string& msg, bool status = true, bool bold_all = false);

string html_tree(const string& path, const vector<string>& sub_dirs);

string html_mail(const string& request, const string& content);

#endif // !_HTML_GENERATOR_H_