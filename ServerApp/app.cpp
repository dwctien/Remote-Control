#include "app.h"

// Helper function to check if a process has a GUI (interactive process)
bool isGuiProcess(DWORD processID) {
    // Start with the first top-level window in the system
    HWND hwnd = FindWindowEx(NULL, NULL, NULL, NULL);
    while (hwnd != NULL) {
        DWORD windowProcessID;
        // Retrieve the process ID that owns the current window
        GetWindowThreadProcessId(hwnd, &windowProcessID);

        // Check if the current window belongs to the specified process
        if (windowProcessID == processID) {
            // Check if the window meets the criteria to appear on the taskbar
            if (IsWindowVisible(hwnd) && GetAncestor(hwnd, GA_ROOT) == hwnd) {
                // Check if the window has a non-empty title
                int length = GetWindowTextLength(hwnd);
                if (length > 0) {
                    // If all conditions are met, the process has a GUI window
                    return true;
                }
            }
        }
        // Move to the next top-level window in the system
        hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
    }
    // If no suitable window is found, return false
    return false;
}

DataFrame creatAppDataFrame(vector<string> appNames, vector<string> appIDs) {
    DataFrame dataFrame;
    dataFrame.columns.push_back("No.");
    dataFrame.columns.push_back("Application");
    dataFrame.columns.push_back("ID");
    dataFrame.type = "single";

    for (int i = 0; i < appNames.size(); i++) {
        vector<string> line;
        line.push_back(to_string(i + 1));
        line.push_back(appNames[i]);
        line.push_back(appIDs[i]);
        dataFrame.data.push_back(line);
    }
    return dataFrame;
}

Response listApp() {
    vector<string> appNames, appIDs, appThreads;
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
            // Check if the process has a GUI window (is an interactive process)
            if (isGuiProcess(pe32.th32ProcessID)) {
                wstring ws(pe32.szExeFile);
                string processName(ws.begin(), ws.end());
                appNames.push_back(processName);
                appIDs.push_back(to_string(pe32.th32ProcessID));
            }
        } while (Process32Next(hSnapshot, &pe32)); // Iterate through processes
    }
    else {
        res.first = "Failed to retrieve process information.";
        return res;
    }

    countThread(appNames, appIDs, appThreads);

    dataFrame = creatAppDataFrame(appNames, appIDs);

    res.first = html_table(dataFrame);
    // Clean up handle
    CloseHandle(hSnapshot);
    return res;
}

Response startApp(string path) {
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
        res.first = html_msg("Failed to start application. Error code: " + to_string((int)result), false, false);
    }
    else {
        res.first = html_msg("Application started successfully.", true, true);
    }

    return res;
}

Response stopApp(int id) {
    Response res;

    // Open a handle to the process using the application ID
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, id);

    // Terminate the application
    if (hProcess == nullptr || !TerminateProcess(hProcess, 0)) {
        res.first = html_msg("Failed to stop application. Error code: " + to_string(GetLastError()), false, false);
    }
    else {
        CloseHandle(hProcess);
        res.first = html_msg("Successfully stopped application.", true, true);
    }
    return res;
}