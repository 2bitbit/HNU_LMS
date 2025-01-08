#pragma once
#include "../models_utils.hpp"

class User {
    // 所有用户而非特定某个用户，所以是静态成员函数
  public:
    // 用户认证
    static void login_user(string account, string pwd) {
        if (account == "")
            throw string("错误：用户账号不能为空");
        Statement query(DB::get_users_db(), "SELECT * FROM users WHERE 账号 = ? AND 密码 = ?");
        query.bind(1, account);
        query.bind(2, pwd);
        if (!query.executeStep())
            throw string("错误：用户账号或密码错误");
    };

    // 功能部分
    // 更改密码
    static void change_pwd(string account, string new_pwd) {
        Statement query(DB::get_users_db(), "UPDATE users SET 密码 = ? WHERE 账号 = ?");
        query.bind(1, hash_pwd(new_pwd));
        query.bind(2, account);
        query.exec();
    }

    // 查看用户借阅上限
    static int get_user_borrow_limit(string account) {
        Statement query(DB::get_users_db(), "SELECT 借阅上限 FROM users WHERE 账号 = ?");
        query.bind(1, account);
        if (!query.executeStep())
            throw string("错误：用户不存在");
        return query.getColumn(0).getInt();
    }

    // 查看用户借阅记录
    static string get_borrow_records(string account) {
        Statement query(DB::get_borrow_records_db(), "SELECT 书名, 作者, 出借时间 FROM borrow_records WHERE 账号 = ?");
        query.bind(1, account);
        string records;
        while (query.executeStep()) {
            records += "书名：" + string(query.getColumn(0).getText()) + "\t" + "作者：" +
                       string(query.getColumn(1).getText()) + "\t" + "出借时间：" +
                       string(query.getColumn(2).getText()) + "\n";
        }
        return records;
    };
};