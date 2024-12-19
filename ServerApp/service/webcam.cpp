#include "../include/webcam.h"

cv::Mat capturePhoto() {
    cv::VideoCapture camera(0); // Open webcam with ID 0
    if (!camera.isOpened()) {
        cerr << "Failed to access the webcam!\n";
        return cv::Mat();
    }

    cv::Mat frame;
    cv::waitKey(500); // Wait 500ms for warm-up
    camera >> frame; // Capture a frame from the webcam

    camera.release();
    return frame;
}

Response getImage(string filename) {
    Response res;

    // Capture the screen and save it as an image file
    cv::Mat mat = capturePhoto();
    
    // Check if the image was successfully captured
    if (mat.empty()) {
        res.first = html_msg("Cannot capture an image from webcam.", false, false);
        return res;
    }

    // Open the file in binary mode
    cv::imwrite(filename, mat);
    ifstream file(filename, ios::binary);
    if (!file) {
        res.first = html_msg("Cannot capture an image from webcam.", false, false);
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

    res.first = html_msg("The image is attached below.", true, true);
    res.second = move(packedData);

    return res;
}

void recordVideo(string filename, int duration) {
    cv::VideoCapture camera(0); // Open webcam with ID 0
    if (!camera.isOpened()) {
        cerr << "Failed to access the webcam!\n";
        return;
    }

    int fps = 20; // Frames per second

    cv::Mat frame;
    camera >> frame; // Capture a frame from the webcam

    string videoFile = filename;
    cv::VideoWriter videoWriter(videoFile, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frame.size());

    if (!videoWriter.isOpened()) {
        cerr << "Failed to create video file!" << endl;
        camera.release();
        return;
    }

    int frame_count = duration * fps;
    for (int i = 0; i < frame_count; ++i) {
        camera >> frame; // Capture a frame from the webcam
        if (frame.empty()) {
            cerr << "Failed to capture a frame from the webcam!" << endl;
            break;
        }

        videoWriter.write(frame); // Write frame to video file

        if (cv::waitKey(1000 / fps) == 27) { // Press ESC to stop
            cout << "Recording stopped." << endl;
            break;
        }
    }

    videoWriter.release(); // Close the video file
    cout << "Video recorded and saved as " << videoFile << endl;

    camera.release();
    cout << "Webcam turned off." << endl;
}

Response getVideo(string filename, int duration) {
    Response res;

    recordVideo(filename, duration);

    // Open the file in binary mode
    ifstream file(filename, ios::binary);
    if (!file) {
        res.first = html_msg("Cannot record a video from webcam.", false, false);
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

    res.first = html_msg("The video is attached below.", true, true);
    res.second = move(packedData);

    return res;
}