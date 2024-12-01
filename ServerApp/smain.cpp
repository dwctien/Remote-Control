#include "file_handler.h"

int main() {
    int choice;
    cout << "Choose an option:\n";
    cout << "1. Copy file\n";
    cout << "2. Delete file\n";
    cout << "Enter your choice (1 or 2): ";
    cin >> choice;

    if (choice == 1) {
        string source, destination;
        cout << "Enter source file path: ";
        cin.ignore();
        getline(cin, source);

        cout << "Enter destination file path: ";
        getline(cin, destination);

        if (!source.empty() && !destination.empty()) {
            copyFile(source, destination);
        }
        else {
            cerr << "Source or destination path is empty.\n";
        }
    }
    else if (choice == 2) {
        char filePath[260];
        cout << "Enter file path to delete: ";
        cin >> filePath;
        deleteFileToRecycleBin(filePath);
    }
    else {
        cerr << "Invalid choice.\n";
    }

    return 0;
}
