#pragma once
#include "../utils.hpp"
#include "DB.hpp"

// 查询是否存在某东西
bool is_exist(string what, string type) {
    Statement query(DB::get_books_db(), "");
    if (type == "title") {
        query = Statement(DB::get_books_db(), "SELECT * FROM books WHERE 书名 = ?");
    } else if (type == "author") {
        query = Statement(DB::get_books_db(), "SELECT * FROM books WHERE 作者 = ?");
    } else if (type == "isxn") {
        query = Statement(DB::get_books_db(), "SELECT * FROM books WHERE ISXN = ?");
    } else if (type == "admin") {
        query = Statement(DB::get_admins_db(), "SELECT * FROM admins WHERE 账号 = ?");
    } else if (type == "user") {
        query = Statement(DB::get_users_db(), "SELECT * FROM users WHERE 账号 = ?");
    } else if (type == "borrow_record") {
        query = Statement(DB::get_borrow_records_db(), "SELECT * FROM borrow_records WHERE 账号 = ?");
    } else if (type == "reach_borrow_limit") {
        query = Statement(DB::get_users_db(), "SELECT 借阅上限 FROM users WHERE 账号 = ?");
        query.bind(1, what);
        query.executeStep();
        int borrow_limit = query.getColumn(0).getInt();
        query = Statement(DB::get_borrow_records_db(), "SELECT COUNT(*) FROM borrow_records WHERE 账号 = ?");
        query.bind(1, what);
        query.executeStep();
        int borrow_count = query.getColumn(0).getInt();
        return borrow_count >= borrow_limit;
    } else {
        throw string("错误：不存在此type：" + type);
    }
    query.bind(1, what);
    return query.executeStep();
}

// 查询是否存在某关系
bool is_exist_relation(string big_a, string small_b, string type) {

    Statement query(DB::get_books_db(), "");
    if (type == "borrow_record") {
        query = Statement(DB::get_borrow_records_db(), "SELECT * FROM borrow_records WHERE 账号 = ? AND ISXN = ?");
    } else {
        throw string("错误：不存在此" + type);
    }
    query.bind(1, big_a);
    query.bind(2, small_b);
    return query.executeStep();
}