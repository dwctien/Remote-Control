#include "file_handler.h"

void copyFile(const string& source, const string& destination) {
    if (CopyFileA(source.c_str(), destination.c_str(), FALSE)) {
        std::cout << "File copied successfully.\n";
    }
    else {
        std::cerr << "Failed to copy file. Error code: " << GetLastError() << "\n";
    }
}

void deleteFile(const char* filePath) {
    if (DeleteFileA(filePath)) {
        std::cout << "File deleted successfully.\n";
    }
    else {
        DWORD error = GetLastError();
        std::cerr << "Failed to delete file. Error code: " << error << "\n";

        if (error == ERROR_ACCESS_DENIED) {
            std::cerr << "Access denied. You might need admin privileges or the file might be in use.\n";
        }
        else if (error == ERROR_FILE_NOT_FOUND) {
            std::cerr << "The file was not found. Please check the file path.\n";
        }
        else if (error == ERROR_SHARING_VIOLATION) {
            std::cerr << "The file is currently in use by another program.\n";
        }
    }
}

wstring stringToWString(const string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
    return wstrTo;
}

void deleteFileToRecycleBin(const char* filePath) {
    // Convert the path from const char* to std::wstring
    wstring wFilePath = stringToWString(filePath) + L'\0';  // Terminate the string with \0

    // Use the SHFILEOPSTRUCT structure to perform the delete operation
    SHFILEOPSTRUCTW fileOp = { 0 };
    fileOp.wFunc = FO_DELETE;                                   // Set the action to delete
    fileOp.pFrom = wFilePath.c_str();                           // Specify the file path to be deleted
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;         // Move the file to the Recycle Bin without requiring confirmation

    if (SHFileOperationW(&fileOp) == 0) {
        std::cout << "File moved to Recycle Bin successfully.\n";
    }
    else {
        std::cerr << "Failed to move file to Recycle Bin.\n";
    }
}