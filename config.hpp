#pragma once
#include <string>
using namespace std;
// 产品密钥
const string product_key = "LOVE";

// 数据库表：分库存储，强化隔离，降低数据泄露风险
const string CREATE_TABLE_BOOKS = "CREATE TABLE IF NOT EXISTS books (id INTEGER PRIMARY KEY, "
                                  "书名 TEXT, 作者 TEXT, 出版社 TEXT, ISXN TEXT, 参考价格 REAL, "
                                  "出版时间 DATE, 介绍 TEXT, 累计借阅次数 INTEGER, 在架数目 INTEGER)";
const string CREATE_TABLE_USERS =
    "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, 账号 TEXT, 密码 TEXT, 借阅上限 INTEGER)";
const string CREATE_TABLE_ADMINS = "CREATE TABLE IF NOT EXISTS admins (id INTEGER PRIMARY KEY, 账号 TEXT, 密码 TEXT, "
                                   "联系信息 TEXT)";
const string CREATE_TABLE_BORROW_RECORDS =
    "CREATE TABLE IF NOT EXISTS borrow_records (id INTEGER PRIMARY KEY, 账号 TEXT, 书名 TEXT, 作者 TEXT, ISXN TEXT, "
    "出借时间 DATE)"; // 存储没还的书。时间格式:YYYY-MM-DD
