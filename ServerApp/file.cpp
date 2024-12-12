#include "file.h"

vector<string> diskList() {
    vector<string> disks;
    for (char c = 'A'; c <= 'Z'; ++c) {
        string path = string(1, c) + ":\\";
        if (fs::exists(path) && fs::is_directory(path)) {
            disks.push_back(path);
        }
    }
    return disks;
}

vector<string> listDir(const string& path) {
    vector<string> result;
    if (path.empty()) {
        vector<string> disks = diskList();
        result.insert(result.end(), disks.begin(), disks.end());
    }
    else {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (const auto& entry : fs::directory_iterator(path)) {
                result.push_back(entry.path().filename().string());
            }
        }
    }
    return result;
}

Response showTree(const string& path = "") {
    vector<string> subDirs = listDir(path);
    string html;
    if (subDirs.empty()) {
        html = "<p>The directory " + path + " does not exist or is empty.</p>";
    }
    else {
        html = html_tree(path, subDirs);
    }

    return { html, vector<BYTE>{} };
}

void copyFile(const string& source, const string& destination) {
    if (CopyFileA(source.c_str(), destination.c_str(), FALSE)) {
        std::cout << "File copied successfully.\n";
    }
    else {
        std::cerr << "Failed to copy file. Error code: " << GetLastError() << "\n";
    }
}

wstring stringToWString(const string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
    return wstrTo;
}

//void deleteFile(const char* filePath) {
//    // Convert the path from const char* to std::wstring
//    wstring wFilePath = stringToWString(filePath) + L'\0';      // Terminate the string with \0
//
//    // Use the SHFILEOPSTRUCT structure to perform the delete operation
//    SHFILEOPSTRUCTW fileOp = { 0 };
//    fileOp.wFunc = FO_DELETE;                                   // Set the action to delete
//    fileOp.pFrom = wFilePath.c_str();                           // Specify the file path to be deleted
//    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;         // Move the file to the Recycle Bin without requiring confirmation
//
//    if (SHFileOperationW(&fileOp) == 0) {
//        cout << "File moved to Recycle Bin successfully.\n";
//    }
//    else {
//        cerr << "Failed to move file to Recycle Bin.\n";
//    }
//}