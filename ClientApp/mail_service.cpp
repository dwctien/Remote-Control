#include "include/mail_service.h"
#include "include/globals.h"

json loadConfig(const string& config_file_path) {
    ifstream config_file(config_file_path);
    if (!config_file.is_open()) {
        throw runtime_error("Could not open config file: " + config_file_path);
    }

    json config;
    config_file >> config;
    return config;
}

string getAccessToken() {
    // Load configuration
    json config = loadConfig();

    string client_id = config["client_id"];
    string client_secret = config["client_secret"];
    string refresh_token_str = config["refresh_token"];

    // Get a fresh access token
    string access_token = refreshToken(client_id, client_secret, refresh_token_str);

    return access_token;
}

string base64_encode_str(const string& in) {
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

    // Convert Base64 to URL-safe Base64
    for (char& c : encoded_data) {
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
    }

    // Remove padding '='
    while (!encoded_data.empty() && encoded_data.back() == '=') {
        encoded_data.pop_back();
    }

    return encoded_data;
}

string base64_encode_bin(const BYTE* data, size_t length) {
    static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    string encoded;
    int val = 0;
    int valb = -6;
    for (size_t i = 0; i < length; ++i) {
        val = (val << 8) + data[i];
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (encoded.size() % 4) {
        encoded.push_back('=');
    }

    return encoded;
}

size_t writeCallback(void* contents, size_t size, size_t nmemb, string* out) {
    size_t totalSize = size * nmemb;
    out->append((char*)contents, totalSize);
    return totalSize;
}

string refreshToken(const string& client_id, const string& client_secret, const string& refresh_token) {
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
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

bool sendMail(const string& recipient, const string& subject, const string& body, const vector<BYTE>& attachment, const string& attachment_filename) {
    try {
        string access_token = getAccessToken();
        
        // Prepare email content
        string boundary = "__boundary__";
        string email = "From: msg.controller@gmail.com\r\n";
        email += "To: " + recipient + "\r\n";
        email += "Subject: " + subject + "\r\n";
        email += "MIME-Version: 1.0\r\n";
        email += "Content-Type: multipart/mixed; boundary=\"" + boundary + "\"\r\n\r\n";

        // Body part
        email += "--" + boundary + "\r\n";
        email += "Content-Type: text/html; charset=UTF-8\r\n";
        email += "Content-Transfer-Encoding: 7bit\r\n\r\n";
        email += body + "\r\n";

        // Attachment part
        if (!attachment.empty()) {
            string encoded_attachment = base64_encode_bin(attachment.data(), attachment.size());

            email += "--" + boundary + "\r\n";
            email += "Content-Type: application/octet-stream; name=\"" + attachment_filename + "\"\r\n";
            email += "Content-Transfer-Encoding: base64\r\n";
            email += "Content-Disposition: attachment; filename=\"" + attachment_filename + "\"\r\n\r\n";
            email += encoded_attachment + "\r\n";
        }

        // End of email
        email += "--" + boundary + "--\r\n";

        string encoded_email = base64_encode_str(email);
        string json_data = R"({ "raw": ")" + encoded_email + R"(" })";

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
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
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

bool readMail(const string& accessToken, const string& messageId, string& subject, string& body, string& admin) {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    // The URL to fetch the email content from Gmail API
    string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId + "?format=full";

    curl = curl_easy_init();
    if (curl) {
        // Setting up the authorization header with the access token
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Sending the request to Gmail API
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            // Parsing the JSON response from Gmail API
            auto jsonResponse = nlohmann::json::parse(readBuffer);

            // Extracting the email subject
            for (const auto& header : jsonResponse["payload"]["headers"]) {
                if (header["name"] == "Subject") {
                    subject = header["value"];
                }
                if (header["name"] == "From") {
                    string from = header["value"];

                    size_t startPos = from.find("<");
                    size_t endPos = from.find(">");

                    if (startPos != string::npos && endPos != string::npos && endPos > startPos) {
                        admin = from.substr(startPos + 1, endPos - startPos - 1);
                    }
                    else {
                        admin = from;
                    }
                }
            }

            // Extracting the body snippet of the email
            body = jsonResponse["snippet"];
            return true; // Successfully fetched email content
        }
        else {
            cerr << "Error fetching email content: " << curl_easy_strerror(res) << endl;
        }

        // Cleaning up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return false; // Failed to fetch email content
}

void markEmailAsRead(const string& accessToken, const string& messageId) {
    string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId + "/modify";
    nlohmann::json payload = { {"removeLabelIds", {"UNREAD"}} };
    string jsonData = payload.dump();

    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        curl_easy_perform(curl);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

vector<string> getAdminList(string filename) {
    vector<string> list = {};

    ifstream ifile;
    ifile.open(filename);
    if (!ifile.is_open()) {
        return list;
    }

    string line = "";
    while (!ifile.eof()) {
        getline(ifile, line);
        if (!line.empty()) {
            list.push_back(line);
        }
    }

    ifile.close();
    return list;
}

bool validateAdmin(string admin) {
    vector<string> adminList = getAdminList();
    if (find(adminList.begin(), adminList.end(), admin) == adminList.end()) {
        return false;
    }
}

bool validateIP(string ip_addr) {
    regex ipRegex(R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)");
    smatch match;
    if (regex_match(ip_addr, match, ipRegex)) {
        for (int i = 1; i <= 4; ++i) {
            int part = stoi(match[i].str());
            if (part < 0 || part > 255) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void checkMail() {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    // Set up the Gmail API URL to fetch unread emails
    string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages?q=is:unread";
    curl = curl_easy_init();

    // Get access token
    string access_token = getAccessToken();

    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);
            if (jsonResponse.contains("messages")) {
                for (const auto& message : jsonResponse["messages"]) {
                    string messageId = message["id"];

                    markEmailAsRead(access_token, messageId);

                    // Get the subject and body
                    string subject, body, admin;
                    bool valid = readMail(access_token, messageId, subject, body, admin);

                    string response_subject = "";
                    string response_body = "";
                    string filename = "";
                    vector <BYTE> response_data = {};

                    if (!validateAdmin(admin)) {
                        response_subject = "Reply for request: " + subject;
                        response_body = html_mail(subject, html_msg("You are not allowed to control this PC.", false, true));
                        sendMail(admin, response_subject, response_body, response_data, filename);
                    }
                    else if (!validateIP(body)) {
                        response_subject = "Reply for request: " + subject;
                        response_body = html_mail(subject, html_msg("Invalid IP address. Make sure the server is on and the server's IP address is correct.", false, true));
                        sendMail(admin, response_subject, response_body, response_data, filename);
                    }
                    else {
                        runClient(subject, body, response_subject, response_body, filename, response_data);

                        cout << "Response mail: " << response_body << "\n";

                        sendMail(admin, response_subject, response_body, response_data, filename);
                    }
                }
            }
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

void checkMailsContinuously() {
    while (isRunning) {
        checkMail();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

// ui-related -------------------------------------------------------------------------
bool isValidEmailAddress(const string& email, vector<string> emailList) {
    const std::regex emailPattern(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
    );
    return std::regex_match(email, emailPattern) &&
        (std::find(emailList.begin(), emailList.end(), email) == emailList.end());
}

void removeMail(const string& email, vector<string>& emailList) {
    auto it = find(emailList.begin(), emailList.end(), email);
    if (it != emailList.end()) {
        emailList.erase(it);
    }
}

bool updateAdmin(vector<string> gmails) {
    ofstream file("admin.txt");
    if (!file.is_open()) {
        cout << "Cannot open admin file" << endl;
        return false;
    }
    else {
        for (int i = 0; i < gmails.size(); i++) {
            file << gmails[i] << endl;
        }
        file.close();
        return true;
    }
}