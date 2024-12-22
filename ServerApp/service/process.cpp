#include "../include/process.h"

DataFrame creatProcessDataFrame(vector<string> processNames, vector<string> processIDs, vector<string> processThreads) {
    DataFrame dataFrame;
    dataFrame.columns.push_back("No.");
    dataFrame.columns.push_back("Process");
    dataFrame.columns.push_back("ID");
    dataFrame.columns.push_back("Thread Count");
    dataFrame.type = "single_process";

    for (int i = 0; i < processNames.size(); i++) {
        vector<string> line;
        line.push_back(to_string(i + 1));
        line.push_back(processNames[i]);
        line.push_back(processIDs[i]);
        line.push_back(processThreads[i]);
        dataFrame.data.push_back(line);
    }
    return dataFrame;
}

void countThread(vector<string>& processNames, vector<string>& processIDs, vector<string>& processThread) {
    vector<string> names, id, thread; // After count thread

    for (int i = 0; i < processNames.size(); i++) {
        bool isCounted = false;
        for (int j = 0; j < names.size(); j++) {
            if (names[j] == processNames[i]) {
                isCounted = true;
                break;
            }
        }

        if (isCounted) {
            continue;
        }

        int count = 0;
        for (int j = i; j < processNames.size(); j++) {
            if (processNames[j] == processNames[i]) {
                count++;
            }
        }

        names.push_back(processNames[i]);
        id.push_back(processIDs[i]);
        thread.push_back(to_string(count));
    }

    processNames = names;
    processIDs = id;
    processThread = thread;
}

Response listProcess() {
    vector<string> processNames, processIDs, processThreads;
    DataFrame dataFrame;
    Response res;

    // Create a snapshot of all processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        res.first = "Failed to create snapshot of processes.";
        return res;
    }

    // Initialize PROCESSENTRY32 structure
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    // Retrieve information about the first process
    if (Process32First(hSnapshot, &pe32)) {
        do {
            // Add process name and ID to the vectors
            wstring ws(pe32.szExeFile);
            string processName(ws.begin(), ws.end());
            processNames.push_back(processName);
            processIDs.push_back(to_string(pe32.th32ProcessID));
        } while (Process32Next(hSnapshot, &pe32)); // Iterate through processes
    }
    else {
        res.first = "Failed to retrieve process information.";
        return res;
    }

    countThread(processNames, processIDs, processThreads);

    dataFrame = creatProcessDataFrame(processNames, processIDs, processThreads);
    
    res.first = html_table(dataFrame);
    // Clean up handle
    CloseHandle(hSnapshot);
    return res;
}

Response startProcess(string path) {
    Response res;
    
    HINSTANCE result = ShellExecuteA(
        nullptr,        // Parent window (nullptr if not needed)
        "open",         // Action to be performed (e.g., "open", "print")
        path.c_str(),   // Path to the file or application
        nullptr,        // Parameters to be passed to the application (if any)
        nullptr,        // Initial working directory (nullptr = default)
        SW_SHOWNORMAL   // Window display option (SW_SHOWNORMAL: normal display)
    );

    if ((int)result <= 32) {
        res.first = html_msg("Failed to start process. Error code: " + to_string((int)result) + ".", false, false);
    }
    else {
        res.first = html_msg("Process started successfully.", true, true);
    }

    return res;
}

Response stopProcess(int processId) {
    Response res;
    
    // Open a handle to the process using the process ID
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);

    if (hProcess == nullptr) {
        res.first = html_msg("Failed to stop process. Error code: " + to_string(GetLastError()) + ".", false, false);
        return res;
    }

    // Terminate the process
    if (!TerminateProcess(hProcess, 0)) {
        res.first = html_msg("Failed to stop process. Error code: " + to_string(GetLastError()) + ".", false, false);
    }
    else {
        res.first = html_msg("Process stopped successfully.", true, true);
    }

    // Close the handle
    CloseHandle(hProcess);

    return res;
}