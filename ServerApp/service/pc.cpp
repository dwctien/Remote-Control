#include "../include/pc.h"

bool ShutdownUsingWinAPI(bool reboot) {
    // Parameters for the shutdown function
    LPWSTR machineName = NULL;          // NULL means the current machine
    LPWSTR message = NULL; // Message displayed to the user before shutdown
    DWORD timeout = 5;                   // Delay time before shutdown in seconds (0 = immediate)
    BOOL forceAppsClosed = TRUE;         // Force all applications to close without saving
    BOOL rebootAfterShutdown = reboot;    // FALSE = Shutdown, TRUE = Restart the machine

    // Request necessary privileges to initiate a system shutdown
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Open the process token to adjust privileges
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false; // Return false if the token could not be opened
    }

    // Lookup the shutdown privilege (SE_SHUTDOWN_NAME) and enable it
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1; // Set the privilege count to 1
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // Enable the privilege

    // Adjust the token privileges for the current process
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS) {
        return false; // Return false if the privilege adjustment fails
    }

    // Call InitiateSystemShutdownEx to perform the shutdown
    if (!InitiateSystemShutdownEx(
        machineName,               // Target machine (NULL = current machine)
        message,                   // Message to display before shutdown
        timeout,                   // Timeout before shutdown (in seconds)
        forceAppsClosed,           // Force applications to close
        rebootAfterShutdown,       // Reboot the system after shutdown
        SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER)) { // Reason for shutdown
        return false; // Return false if the shutdown command fails
    }

    return true; // Return true if the shutdown is successfully initiated
}

Response shutdownPC() {
    Response succ, fail;
    succ.first = html_msg("Shutdown successful.", true, true);
    fail.first = html_msg("Shutdown failed.", false, false);

    if (ShutdownUsingWinAPI(false)) {
        return succ;
    }
    else {
        return fail;
    }
}

Response restartPC() {
    Response succ, fail;
    succ.first = html_msg("Restart successful.", true, true);
    fail.first = html_msg("Restart failed.", false, false);

    if (ShutdownUsingWinAPI(true)) {
        return succ;
    }
    else {
        return fail;
    }
}