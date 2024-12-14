#ifndef _REQUEST_HANDLE_H_
#define _REQUEST_HANDLE_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iomanip>

#include "file.h"
#include "screen.h"
#include "webcam.h"
#include "keyboard.h"
#include "process.h"
#include "app.h"

const string APP_REQ = "[ctrl]"; // Prefix to validate commands

typedef pair<string, vector<BYTE>> Response;

using Function = function<Response(vector<string>)>;

// Request tree with various commands (using lambda functions to wrap the actual functions)
extern map<string, map<string, pair<int, Function>>> requestTree;

extern vector<string> whiteList;

vector<string> tokenize(const string& str);

map<string, string> parseRequest(const string& sender, const string& subject, Function& outFunction, vector<string>& outParams);

vector<string> loadAdmin();

bool addAdmin(string gmailAdmin);

#endif // !_REQUEST_HANDLE_H_