#ifndef _HTML_GENERATOR_H_
#define _HTML_GENERATOR_H_

#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <algorithm>

using namespace std;

typedef unsigned char BYTE;
typedef pair<string, vector<BYTE>> Response;

string html_msg(const string& msg, bool status = true, bool bold_all = false);

string html_mail(const string& request, const string& content);

#endif // !_HTML_GENERATOR_H_