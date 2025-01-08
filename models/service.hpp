#pragma once
#include "models_utils.hpp"
// 服务层：处理其他业务逻辑。
using namespace std;
using namespace ftxui;

// 湖机儿服务
string get_response(string prompt) {
    string command = string("python ") + dir + "ai.py " + prompt;
    system(command.c_str());
    ifstream tmp("chat_history.txt");
    string line;
    string lastLine;
    while (getline(tmp, line)) {
        lastLine = line;
    }
    // 输出最后一行
    if (!lastLine.empty()) {
        string result = lastLine.substr(lastLine.find("|") + 1);
        return regex_replace(result, regex("<br>"), "\n");
    } else {
        return "";
    }
}
