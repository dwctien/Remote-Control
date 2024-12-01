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

using json = nlohmann::json;
using namespace std;

// Load configuration from `config.json`
json load_config(const string& config_file_path = "config.json");

// Callback for handling response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* out);

string refresh_token(const string& client_id, const string& client_secret, const string& refresh_token);

bool send_email(const string& recipient, const string& subject, const string& body);

#endif // !_MAIL_SERVICE_H_