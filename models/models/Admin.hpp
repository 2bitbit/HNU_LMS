#pragma once
#include "../models_utils.hpp"

class Admin {
    // 所有管理员而非特定某个管理员，所以是静态成员函数
  public:
    // 注册登录部分
    // 添加管理员
    static void register_admin(string account, string pwd, string contact_info, string key) {
        // 注册检查
        if (account == "")
            throw string("错误：管理员账号不能为空");
        else if (pwd == "")
            throw string("错误：管理员密码不能为空");
        else if (key != product_key)
            throw string("错误：密钥错误");
        else if (is_exist(account, "admin")) {
            throw string("错误：已存在该管理员账号");
        }
        // 正式注册
        Statement query(DB::get_admins_db(), "INSERT INTO admins (账号, 密码, 联系信息) VALUES (?, ?, ?)");
        query.bind(1, account);
        query.bind(2, pwd);
        query.bind(3, contact_info);
        query.exec();
    };

    // 管理员认证
    static void login_admin(string account, string pwd) {
        Statement query(DB::get_admins_db(), "SELECT * FROM admins WHERE 账号 = ? AND 密码 = ?");
        query.bind(1, account);
        query.bind(2, pwd);
        if (!query.executeStep())
            throw string("错误：管理员账号或密码错误");
    }

    // 获取管理员联系方式
    static string get_admins_contact() {
        Statement query(DB::get_admins_db(), "SELECT 账号, 联系信息 FROM admins");
        string contact = "";
        while (query.executeStep())
            contact += string(query.getColumn(0).getText()) + "：" + string(query.getColumn(1).getText()) + "\n\n";
        return contact;
    }

    // 用户管理
    // 借阅图书
    static void borrow_book(string user_account, string isxn) {
        if (!is_exist(user_account, "user"))
            throw string("错误：用户不存在");
        else if (!is_exist(isxn, "isxn"))
            throw string("错误：图书不存在");
        else if (is_exist_relation(user_account, isxn, "borrow_record"))
            throw string("错误：已借阅该图书");
        else if (is_exist(user_account, "reach_borrow_limit"))
            throw string("错误：已达到借阅上限");
        Statement tmp_query(DB::get_books_db(), "SELECT 书名, 作者 FROM books WHERE ISXN = ?");
        tmp_query.bind(1, isxn);
        tmp_query.executeStep();
        string title = tmp_query.getColumn(0).getText();
        string author = tmp_query.getColumn(1).getText();
        Statement query(
            DB::get_borrow_records_db(),
            "INSERT INTO borrow_records (账号, 书名, 作者, ISXN, 出借时间) VALUES (?, ?, ?, ?, CURRENT_TIMESTAMP)");
        query.bind(1, user_account);
        query.bind(2, title);
        query.bind(3, author);
        query.bind(4, isxn);
        query.exec();
        // 被借阅书籍次数+1,在架数目-1
        Statement query_borrow_times(
            DB::get_books_db(),
            "UPDATE books SET 累计借阅次数 = 累计借阅次数 + 1, 在架数目 = 在架数目 - 1 WHERE ISXN = ?");
        query_borrow_times.bind(1, isxn);
        query_borrow_times.exec();
    };

    // 归还图书
    static void return_book(string user_account, string isxn) {
        if (!is_exist(user_account, "user"))
            throw string("错误：用户不存在");
        else if (!is_exist(isxn, "isxn"))
            throw string("错误：图书不存在");
        else if (!is_exist_relation(user_account, isxn, "borrow_record"))
            throw string("错误：未借阅该图书");
        Statement query(DB::get_borrow_records_db(), "DELETE FROM borrow_records WHERE 账号 = ? AND ISXN = ?");
        query.bind(1, user_account);
        query.bind(2, isxn);
        query.exec();
        // 在架数目+1
        Statement query_number_in_stock(DB::get_books_db(), "UPDATE books SET 在架数目 = 在架数目 + 1 WHERE ISXN = ?");
        query_number_in_stock.bind(1, isxn);
        query_number_in_stock.exec();
    };

    // 注册用户：初始密码为123456
    static void register_user(string user_account) {
        if (is_exist(user_account, "user"))
            throw string("错误：已存在该用户账号");
        else if (user_account == "")
            throw string("错误：用户账号不能为空");
        Statement query(DB::get_users_db(), "INSERT INTO users (账号, 密码, 借阅上限) VALUES (?, ?, 2)");
        query.bind(1, user_account);
        query.bind(2, hash_pwd("123456"));
        query.exec();
    }

    // 删除用户
    static void rm_user(string user_account) {
        if (!is_exist(user_account, "user"))
            throw string("错误：用户不存在");
        else if (is_exist(user_account, "borrow_record"))
            throw string("错误：用户存在借阅记录，无法删除");
        else {
            // 删除用户
            Statement query(DB::get_users_db(), "DELETE FROM users WHERE 账号 = ?");
            query.bind(1, user_account);
            query.exec();
        }
    }

    // 查看用户借阅记录
    static string get_borrow_records(string user_account) {
        if (!is_exist(user_account, "user"))
            throw string("错误：用户不存在");
        Statement query(DB::get_borrow_records_db(),
                        "SELECT 书名, 作者, ISXN, 出借时间 FROM borrow_records WHERE 账号 = ?");
        query.bind(1, user_account);
        string record = "";
        while (query.executeStep())
            record += "书名：" + string(query.getColumn(0).getText()) + "\t" + "作者：" +
                      string(query.getColumn(1).getText()) + "\t" + "ISBN/ISSN：" +
                      string(query.getColumn(2).getText()) + "\t" + "出借时间：" +
                      string(query.getColumn(3).getText()) + "\n";
        return record;
    }

    // 重置用户密码为123456
    static void reset_user_pwd(string user_account) {
        if (!is_exist(user_account, "user"))
            throw string("错误：用户不存在");
        // 重置密码
        Statement query(DB::get_users_db(), "UPDATE users SET 密码 = ? WHERE 账号 = ?");
        query.bind(1, hash_pwd("123456"));
        query.bind(2, user_account);
        query.exec();
    }

    // 设置用户借阅上限
    static void set_user_borrow_limit(string user_account, int limit) {
        if (!is_exist(user_account, "user"))
            throw string("错误：用户不存在");
        else if (limit < 0)
            throw string("错误：借阅上限不能为负");
        Statement query(DB::get_users_db(), "UPDATE users SET 借阅上限 = ? WHERE 账号 = ?");
        query.bind(1, limit);
        query.bind(2, user_account);
        query.exec();
    }

    // 图书管理
    // 添加新图书
    static void add_new_book(string title, string author, string publisher, string ISXN, double price,
                             string publication_date, string intro, int number_in_stock) {
        if (title == "" || ISXN == "")
            throw string("错误：书名，ISBN/ISSN，不能为空");
        else if (is_exist(ISXN, "isxn"))
            throw string("错误：已存在该图书，请前往增减图书页面增加数量");
        else if (publication_date != "" && !regex_match(publication_date, regex(R"(\d{4}-\d{2}-\d{2})")))
            throw string("错误：出版时间格式错误");
        Statement query(DB::get_books_db(),
                        "INSERT INTO books (书名, 作者, 出版社, ISXN, 参考价格, 出版时间, 介绍, 累计借阅次数, "
                        "在架数目) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.bind(1, title);
        query.bind(2, author);
        query.bind(3, publisher);
        query.bind(4, ISXN);
        query.bind(5, price);
        query.bind(6, publication_date);
        query.bind(7, intro);
        query.bind(8, 0);
        query.bind(9, number_in_stock);
        query.exec();
    }

    // 增加已有的图书：通过ISBN/ISSN
    static void add_existing_book(string isxn, int number_to_add) {
        if (isxn == "")
            throw string("错误：ISBN/ISSN不能为空");
        else if (number_to_add == 0)
            throw string("错误：增加数量不能为空");
        else if (number_to_add < 0)
            throw string("错误：增加数量不能为负");
        else if (!is_exist(isxn, "isxn"))
            throw string("错误：图书不存在");
        Statement query(DB::get_books_db(), "UPDATE books SET 在架数目 = 在架数目 + ? WHERE ISXN = ?");
        query.bind(1, number_to_add);
        query.bind(2, isxn);
        query.exec();
    }

    // 删除图书：在架数目<0时删除这本书：通过ISBN/ISSN
    static void rm_book(string isxn, int number_to_rm) {
        if (isxn == "")
            throw string("错误：ISBN/ISSN不能为空");
        else if (number_to_rm == 0)
            throw string("错误：删除数量不能为空");
        else if (number_to_rm < 0)
            throw string("错误：删除数量不能为负");
        else if (!is_exist(isxn, "isxn"))
            throw string("错误：图书不存在");
        // 获取在架数目
        Statement query(DB::get_books_db(), "SELECT 在架数目 FROM books WHERE ISXN = ?");
        query.bind(1, isxn);
        query.executeStep();
        int number_in_stock = query.getColumn(0).getInt();
        number_in_stock -= number_to_rm;
        if (number_in_stock < 0) {
            Statement query(DB::get_books_db(), "DELETE FROM books WHERE ISXN = ?");
            query.bind(1, isxn);
            query.exec();
        } else {
            Statement query(DB::get_books_db(), "UPDATE books SET 在架数目 = ? WHERE ISXN = ?");
            query.bind(1, number_in_stock);
            query.bind(2, isxn);
            query.exec();
        }
    }

    // 更新图书信息：以ISXN为唯一标识
    static void update_book_info(string isxn, string title, string author, string publisher, double price,
                                 string publish_date, string intro, int cum_borrowings_number, int number_in_stock) {
        if (isxn == "")
            throw string("错误：ISBN/ISSN不能为空");
        else if (!is_exist(isxn, "isxn"))
            throw string("错误：图书不存在");
        // 先备份旧书信息
        Statement query_backup(DB::get_books_db(), "SELECT * FROM books WHERE ISXN = ?");
        query_backup.bind(1, isxn);
        query_backup.executeStep();
        string title_backup = query_backup.getColumn(1).getText();
        string author_backup = query_backup.getColumn(2).getText();
        string publisher_backup = query_backup.getColumn(3).getText();
        double price_backup = query_backup.getColumn(5).getDouble();
        string publish_date_backup = query_backup.getColumn(6).getText();
        string intro_backup = query_backup.getColumn(7).getText();
        int cum_borrowings_number_backup = query_backup.getColumn(8).getInt();
        int number_in_stock_backup = query_backup.getColumn(9).getInt();

        // 修改图书信息
        Statement query(DB::get_books_db(),
                        "UPDATE books SET 书名 = ?, 作者 = ?, 出版社 = ?, 参考价格 = ?, 出版时间 = ?, 介绍 = ?, "
                        "累计借阅次数 = ?, 在架数目 = ? WHERE ISXN = ?");
        query.bind(1, title == "" ? title_backup : title);
        query.bind(2, author == "" ? author_backup : author);
        query.bind(3, publisher == "" ? publisher_backup : publisher);
        query.bind(4, price == -1 ? price_backup : price);
        query.bind(5, publish_date == "" ? publish_date_backup : publish_date);
        query.bind(6, intro == "" ? intro_backup : intro);
        query.bind(7, cum_borrowings_number == -1 ? cum_borrowings_number_backup : cum_borrowings_number);
        query.bind(8, number_in_stock == -1 ? number_in_stock_backup : number_in_stock);
        query.bind(9, isxn);
        query.exec();
    }
};