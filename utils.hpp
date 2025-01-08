#pragma once
#define WIN32_LEAN_AND_MEAN
#include "config.hpp"
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <ftxui/component/component.hpp> // for Renderer, ResizableSplitBottom, ResizableSplitLeft, ResizableSplitRight, ResizableSplitTop, Container, Button, Input, Menu, ...
#include <ftxui/component/component_base.hpp>     // for ComponentBase
#include <ftxui/component/screen_interactive.hpp> // for ScreenInteractive
#include <ftxui/dom/elements.hpp>        // for Element, operator|, operator|=, text, center, border, bgcolor, bold
#include <ftxui/dom/linear_gradient.hpp> // for LinearGradient::Stop, LinearGradient
#include <ftxui/screen/color.hpp>        // for Color, Color::DeepPink1, Color::DeepSkyBlue1, Color::Yellow
#include <iostream>
#include <memory> // for shared_ptr
#include <openssl/sha.h>
#include <regex>
#include <stack>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>
using namespace std;

// 别的文件include utils.hpp，它只include config.hpp
// 通用工具函数与工具变量：
// 工具函数：得到exe文件路径
string get_exe_path() {
    char ExeFile[256];
    GetModuleFileName(NULL, ExeFile, 200);
    return ExeFile;
}

// 工具变量：exe文件路径
string dir = filesystem::path(get_exe_path()).parent_path().string() + "/";

// 工具函数：判断一个字符是否是中文字符或中文标点或阿拉伯数字
bool is_chinese_char(char32_t ch) {
    // 中文字符的Unicode范围
    if (ch >= 0x4E00 && ch <= 0x9FFF) {
        return true;
    }
    // 中文标点的Unicode范围
    if ((ch >= 0x3000 && ch <= 0x303F) || (ch >= 0xFF00 && ch <= 0xFFEF)) {
        return true;
    }
    return false;
}

// 工具函数：不要以明文存储密码（OpenSSL库）。前端用来加密，后端用来验证。
// 使用盐值hnu1314520，防止彩虹表攻击。除非对方跟我一样喜欢hnu
string hash_pwd(string pwd) {
    string salted_input = "hnu1314520" + pwd;
    unsigned char hash[SHA256_DIGEST_LENGTH]; // 存储哈希结果
    SHA256_CTX sha256;
    SHA256_Init(&sha256);                                              // 初始化哈希上下文
    SHA256_Update(&sha256, salted_input.c_str(), salted_input.size()); // 添加数据
    SHA256_Final(hash, &sha256);                                       // 计算哈希值

    // 将哈希值转换为十六进制字符串
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// 工具函数：将一个字符串补齐成指定长度
// utf-8中大多汉字占3个字节，英文，阿拉伯数字占1个字节。
// 由于汉字显示是2个字符宽度，在para里又对每个汉字多添了1个空格，补齐了3-2=1的这个缺口，所以para处理前后总的宽度仍保持一致，能实现对齐。
string pad_str(string str, int byte_length) {
    return str + string(byte_length - str.size(), ' ');
}