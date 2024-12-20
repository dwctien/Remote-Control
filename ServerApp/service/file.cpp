#include "../include/file.h"

vector<string> diskList() {
    vector<string> disks;
    for (char c = 'A'; c <= 'Z'; ++c) {
        string path = string(1, c) + ":/";
        if (fs::exists(path) && fs::is_directory(path)) {
            disks.push_back(path);
        }
    }
    return disks;
}

vector<string> listDir(string& path) {
    vector<string> result;
    if (path == "0") {
        path = "This PC";
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

Response showTree(string& path) {
    vector<string> subDirs = listDir(path);
    string html;
    if (subDirs.empty()) {
        html = html_msg("The directory " + path + " does not exist or is empty.", false, false);
    }
    else {
        html = html_tree(path, subDirs);
    }
    return { html, vector<BYTE>{} };
}

Response copyFile(const string& source, const string& destination) {
    string msg;
    if (CopyFileA(source.c_str(), destination.c_str(), FALSE)) {
        msg = html_msg("The file has been successfully copied.", true, true);
    }
    else {
        msg = html_msg("Failed to copy the file.", false, false);
    }
    return { msg, vector<BYTE>{} };
}

Response deleteFilePermanently(const string& path) {
    string msg;
    if (DeleteFileA(path.c_str())) {
        msg = html_msg("File deleted successfully.", true, true);
    }
    else {
        DWORD error = GetLastError();
        if (error == ERROR_ACCESS_DENIED) {
            msg = html_msg("Access denied.", false, false);
        }
        else if (error == ERROR_FILE_NOT_FOUND) {
            msg = html_msg("The file was not found.", false, false);
        }
        else if (error == ERROR_SHARING_VIOLATION) {
            msg = html_msg("The file is currently in use by another program.", false, false);
        }
    }
    return { msg, vector<BYTE>{} };
}

wstring stringToWString(const string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wstring wstrTo(size_needed - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
    return wstrTo;
}

Response deleteFileToBin(const string& path) {
    string msg;

    // Convert the path from const char* to std::wstring
    wstring wpath = stringToWString(path.c_str()) + L'\0';      // Terminate the string with \0

    // Use the SHFILEOPSTRUCT structure to perform the delete operation
    SHFILEOPSTRUCTW fileOp = { 0 };
    fileOp.wFunc = FO_DELETE;                                   // Set the action to delete
    fileOp.pFrom = wpath.c_str();                               // Specify the file path to be deleted
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;         // Move the file to the Recycle Bin without requiring confirmation

    if (SHFileOperationW(&fileOp) == 0) {
        msg = html_msg("File moved to Recycle Bin successfully.", true, true);
    }
    else {
        msg = html_msg("Failed to move file to Recycle Bin.", false, false);
    }

    return { msg, vector<BYTE>{} };
}

Response getFile(const string& path) {
    Response res;

    fs::path file_path(path);
    file_path = file_path.make_preferred(); // Convert the path separator to the system's standard format

    string filename = file_path.filename().string();
    ifstream file(file_path, ios::binary);
    if (!file) {
        res.first = html_msg("Cannot get the file.", false, false);
        return res;
    }

    // Pack the filename and file data into a single vector<BYTE>
    vector<BYTE> packedData;

    // Add the length of the filename (4 bytes, big-endian)
    uint32_t filenameLength = static_cast<uint32_t>(filename.size());
    packedData.push_back((filenameLength >> 24) & 0xFF); // Most significant byte
    packedData.push_back((filenameLength >> 16) & 0xFF); // Second byte
    packedData.push_back((filenameLength >> 8) & 0xFF);  // Third byte
    packedData.push_back(filenameLength & 0xFF);         // Least significant byte

    // Add the filename as bytes
    packedData.insert(packedData.end(), filename.begin(), filename.end());

    // Add the file data
    packedData.insert(packedData.end(), istreambuf_iterator<char>(file), istreambuf_iterator<char>());

    file.close();

    res.first = html_msg("The file is attached below.", true, true);
    res.second = move(packedData);

    return res;
}