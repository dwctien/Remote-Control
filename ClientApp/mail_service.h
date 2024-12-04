#ifndef _MAIL_SERVICE_H_
#define _MAIL_SERVICE_H_

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <nlohmann/json.hpp>
#include <regex>
#include <thread>

using json = nlohmann::json;
using namespace std;

// Load configuration from `config.json`
json load_config(const string& config_file_path = "config.json");

// Callback for handling response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* out);

string refresh_token(const string& client_id, const string& client_secret, const string& refresh_token);

bool send_mail(const string& recipient, const string& subject, const string& body);

// Get the subject and body of an email
bool read_mail(const std::string& accessToken, const std::string& messageId, std::string& subject, std::string& body);

// Mark email as read
void markEmailAsRead(const string& accessToken, const string& messageId);

// Validate the email for [ctrl] in subject and IP in body
bool validateEmail(string& subject, string& body);

// Send data to server
void sendToServer(const string& ip, const string& subject, const string& body);

// Check for new emails, mark as read, validate, and send to server
void processEmails(const string& accessToken);

// Function to continuously check for new emails
void checkEmailsContinuously(const string& accessToken);

#endif // !_MAIL_SERVICE_H_