#ifndef _FILE_HANDLER_H_
#define _FILE_HANDLER_H_

#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include <string>

using namespace std;

void copyFile(const string& source, const string& destination);

void deleteFile(const char* filePath);

// Convert from const char* to std::wstring
wstring stringToWString(const string& str);

void deleteFileToRecycleBin(const char* filePath);

#endif // !_FILE_HANDLER_H_
