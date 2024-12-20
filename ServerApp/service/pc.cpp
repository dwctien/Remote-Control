#include "../include/pc.h"

bool ShutdownUsingPowerShell() {
    const char* command = "powershell.exe -Command \"Stop-Computer -Force\"";

    int result = system(command);

    if (result == 0) {
        return true; 
    }
    else {
        return false;
    }
}

bool RestartUsingPowerShell() {
    const char* command = "powershell.exe -Command \"Restart-Computer -Force\"";

    int result = system(command);

    if (result == 0) {
        return true;
    }
    else {
        return false;
    }
}

Response shutdownPC() {
    Response succ, fail;
    succ.first = html_msg("Shutdown successful", true, true);
    fail.first = html_msg("Shutdown failed", false, false);

    if (ShutdownUsingPowerShell()) {
        return succ;
    }
    else {
        return fail;
    }
}

Response restartPC() {
    Response succ, fail;
    succ.first = html_msg("Restart successful", true, true);
    fail.first = html_msg("Restart failed", false, false);

    if (RestartUsingPowerShell()) {
        return succ;
    }
    else {
        return fail;
    }
}