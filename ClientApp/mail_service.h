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

string base64_encode_str(const string& in);

string base64_encode_bin(const BYTE* data, size_t size);

// Callback for handling response data
size_t writeCallback(void* contents, size_t size, size_t nmemb, string* out);

string refreshToken(const string& client_id, const string& client_secret, const string& refresh_token);

bool sendMail(const string& recipient, const string& subject, const string& body, const vector<BYTE>& attachment, const string& attachment_filename);

bool readMail(const string& accessToken, const string& messageId, string& subject, string& body, string& admin);

void markEmailAsRead(const string& accessToken, const string& messageId);

bool validateIP(string ip_addr);

void checkMail();

void checkMailsContinuously();

#endif // !_MAIL_SERVICE_H_