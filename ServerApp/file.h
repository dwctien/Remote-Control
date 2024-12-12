#ifndef _FILE_H_
#define _FILE_H_

#include <iostream>
#include <filesystem>
#include <sstream>
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <algorithm>
#include "html_generator.h"

namespace fs = std::filesystem; // from C++17

vector<string> diskList();

vector<string> listDir(const string& path = "");

Response showTree(const string& path);

void copyFile(const string& source, const string& destination);

wstring stringToWString(const string& str);

//void deleteFile(const char* filePath);

#endif // !_FILE_H_
