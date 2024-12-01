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

// Function to get the subject and body of an email
bool getEmailContent(const std::string& accessToken, const std::string& messageId, std::string& subject, std::string& body) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    // The URL to fetch the email content from Gmail API
    std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId + "?format=full";

    curl = curl_easy_init();
    if (curl) {
        // Setting up the authorization header with the access token
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
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
                    break;
                }
            }

            // Extracting the body snippet of the email
            body = jsonResponse["snippet"];
            return true; // Successfully fetched email content
        }
        else {
            std::cerr << "Error fetching email content: " << curl_easy_strerror(res) << std::endl;
        }

        // Cleaning up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return false; // Failed to fetch email content
}

// Function to extract IP from the body
string extractIP(const string& body) {
    regex ipRegex(R"((\d{1,3}\.){3}\d{1,3})");  // Regex pattern to find IP address
    smatch match;

    if (regex_search(body, match, ipRegex)) {
        return match.str(0);  // Return the first matched IP address
    }
    return "";  // If no IP is found, return an empty string
}

// Function A: Check for new emails, mark as read, validate, and send to server
void processEmails(const string& accessToken) {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    // Set up the Gmail API URL to fetch unread emails
    string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages?q=is:unread";
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);
            if (jsonResponse.contains("messages")) {
                for (const auto& message : jsonResponse["messages"]) {
                    string messageId = message["id"];

                    // Call B to mark the email as read
                    markEmailAsRead(accessToken, messageId);

                    // Get the email details (subject and content)
                    string subject, body;
                    bool valid = getEmailContent(accessToken, messageId, subject, body);

                    // Call C to check conditions
                    if (validateEmail(subject, body)) {
                        // Extract the IP address from the body
                        string ip = extractIP(body);

                        // If an IP was found, remove it from the body
                        if (!ip.empty()) {
                            body = std::regex_replace(body, regex(ip), ""); // Remove the IP address from the body
                        }

                        // Call D to send data to the server with the updated body
                        sendToServer(ip, subject, body);
                    }
                }
            }
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// Function B: Mark email as read
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

// Function C: Validate the email for [ctrl] in subject and IP in body
bool validateEmail(string& subject, string& body) {
    if (subject.find("[ctrl]") == string::npos) {
        return false;
    }

    subject.erase(0, 6); // Remove [ctrl] prefix
    regex ipRegex(R"((\d{1,3}\.){3}\d{1,3})");
    smatch match;
    if (regex_search(body, match, ipRegex)) {
        return true;
    }

    return false;
}

// Function D: Send data to server
void sendToServer(const string& ip, const string& subject, const string& body) {
    CURL* curl;
    CURLcode res;
    string url = "http://" + ip + ":8080";  // Server address and port 8080

    // Combine subject and body into a single string
    string data = subject + " " + body;

    curl = curl_easy_init();  // Initialize cURL
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: text/plain");  // Set content type to text/plain

        // Set the URL and the data to send
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);  // Set headers

        // Perform the HTTP POST request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Error sending data: " << curl_easy_strerror(res) << endl;  // Print error if any
        }

        // Free memory after sending
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);  // Clean up cURL resources
    }
}

// Function to continuously check for new emails
void checkEmailsContinuously(const string& accessToken) {
    while (true) {  // Infinite loop to continuously check for new emails
        processEmails(accessToken);  // Process new emails

        // Wait for a certain period before checking again (e.g., 1 minute)
        this_thread::sleep_for(chrono::seconds(1)); // Adjust the duration as needed
    }
}