#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include "html_generator.h"

Response listProcess();

Response startProcess(string path);

Response stopProcess(int id);

void countThread(vector<string>& processNames, vector<string>& processIDs, vector<string>& processThread);

#endif // !_PROCESS_H_