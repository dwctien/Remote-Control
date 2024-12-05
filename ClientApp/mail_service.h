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
#include "csocket.h"

using json = nlohmann::json;
using namespace std;

json loadConfig(const string& config_file_path = "config.json");

string getAccessToken();

// Callback for handling response data
size_t writeCallback(void* contents, size_t size, size_t nmemb, string* out);

string refreshToken(const string& client_id, const string& client_secret, const string& refresh_token);

bool sendMail(const string& recipient, const string& subject, const string& body);

bool readMail(const string& accessToken, const string& messageId, string& subject, string& body);

void markEmailAsRead(const string& accessToken, const string& messageId);

// Validate the email for [ctrl] in subject and IP in body
bool validateEmail(string& subject, string& body);

void sendToServer(const string& ip, const string& subject, const string& body);

void handleRequest();

void checkMailsContinuously();

#endif // !_MAIL_SERVICE_H_