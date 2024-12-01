#include "mail_service.h"

json load_config(const string& config_file_path) {
    ifstream config_file(config_file_path);
    if (!config_file.is_open()) {
        throw runtime_error("Could not open config file: " + config_file_path);
    }

    json config;
    config_file >> config;
    return config;
}

string base64_encode(const string& in) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);  // Avoid newline characters
    bio = BIO_push(b64, bio);

    BIO_write(bio, in.c_str(), in.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    string encoded_data(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return encoded_data;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* out) {
    size_t totalSize = size * nmemb;
    out->append((char*)contents, totalSize);
    return totalSize;
}

string refresh_token(const string& client_id, const string& client_secret, const string& refresh_token) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw runtime_error("Failed to initialize CURL");
    }

    string response_data;
    string url = "https://oauth2.googleapis.com/token";
    string post_data = "client_id=" + client_id + "&client_secret=" + client_secret + "&refresh_token=" + refresh_token + "&grant_type=refresh_token";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        throw runtime_error("CURL request failed: " + string(curl_easy_strerror(res)));
    }

    curl_easy_cleanup(curl);

    // Parse JSON response to extract access token
    json response_json = json::parse(response_data);
    if (response_json.contains("access_token")) {
        return response_json["access_token"];
    }
    else {
        throw runtime_error("Failed to retrieve access token from response");
    }
}

bool send_email(const string& recipient, const string& subject, const string& body) {
    try {
        // Load configuration
        json config = load_config();

        string client_id = config["client_id"];
        string client_secret = config["client_secret"];
        string refresh_token_str = config["refresh_token"];

        // Get a fresh access token
        string access_token = refresh_token(client_id, client_secret, refresh_token_str);

        // Prepare email content
        string email = "From: msg.controller@gmail.com\r\n";
        email += "To: " + recipient + "\r\n";
        email += "Subject: " + subject + "\r\n";
        email += "Content-Type: text/html; charset=UTF-8\r\n";
        email += "\r\n";
        email += body;

        string encoded_email = base64_encode(email);
        string json_data = "{ \"raw\": \"" + encoded_email + "\" }";

        // Send email
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw runtime_error("Failed to initialize CURL");
        }

        string response_data;
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, "https://gmail.googleapis.com/gmail/v1/users/me/messages/send");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            throw runtime_error("CURL request failed: " + string(curl_easy_strerror(res)));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        cout << "Email sent successfully. Response: " << response_data << endl;
        return true;

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return false;
    }
}