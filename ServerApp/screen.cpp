#include "screen.h"

cv::Mat captureScreen() {
    SetProcessDPIAware(); // Enable DPI-awareness for accurate screen metrics

    // Get the screen dimensions
    HWND hwndDesktop = GetDesktopWindow();
    HDC hdcDesktop = GetDC(hwndDesktop);
    HDC hdcMem = CreateCompatibleDC(hdcDesktop);

    // Get the screen size, not just the client area
    UINT dpi = GetDpiForSystem();
    int width = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
    int height = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

    // Create a bitmap and copy the screen data into it
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcDesktop, width, height);
    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, width, height, hdcDesktop, 0, 0, SRCCOPY);

    // Retrieve bitmap information
    BITMAP bmp;
    GetObject(hBitmap, sizeof(bmp), &bmp);
    cv::Mat img(height, width, CV_8UC4); // CV_8UC4 to store BGRA data

    // Copy bitmap data into cv::Mat
    GetBitmapBits(hBitmap, bmp.bmWidthBytes * bmp.bmHeight, img.data);

    // Clean up resources
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwndDesktop, hdcDesktop);

    // Convert from BGRA to BGR
    cv::Mat imgBGR;
    cv::cvtColor(img, imgBGR, cv::COLOR_BGRA2BGR);

    return imgBGR;
}

Response getScreenshot(string filename) {
    Response res;

    // Capture the screen and save it as an image file
    cv::Mat mat = captureScreen();
    cv::imwrite(filename, mat);

    // Check if the screenshot was successfully captured
    if (mat.empty()) {
        res.first = html_msg("Cannot get the screenshot.", false, false);
        return res;
    }

    // Open the file in binary mode
    ifstream file(filename, ios::binary);
    if (!file) {
        res.first = html_msg("Cannot get the screenshot.", false, false);
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

    res.first = html_msg("The screenshot is attached below.", true, true);
    res.second = move(packedData);

    return res;
}