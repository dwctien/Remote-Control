#ifndef _FILE_H_
#define _FILE_H_

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <algorithm>
#include "html_generator.h"

#pragma comment(lib, "Shell32.lib") 

namespace fs = std::filesystem; // from C++17

vector<string> diskList();

vector<string> listDir(const string& path = "");

Response showTree(const string& path = "");

Response copyFile(const string& source, const string& destination);

Response deleteFilePermanently(const string& path);

wstring stringToWString(const string& str);

Response deleteFileToBin(const string& path);

Response getFile(const string& path);

#endif // !_FILE_H_