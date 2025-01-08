#pragma once
#include "../config.hpp"
#include "../utils.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
using namespace SQLite;

// 通用的底层数据库
class DB {
  private:
    // 由于Database没有默认构造函数，不能默认初始化。
    // 这里使用指针：延迟实例对象的成员变量的初始化：当DB类被实例化，执行初始化创建db文件后，这些成员变量才会被初始化
    shared_ptr<Database> books_db_ptr;
    shared_ptr<Database> users_db_ptr;
    shared_ptr<Database> admins_db_ptr;
    shared_ptr<Database> borrow_records_db_ptr;

    // 单例模式：tui调用models，models访问DB。初次访问时才构建实例并在构造函数内执行初始化操作。
    // 外部无法获得单例，只能是内部使用单例。
    // 让外部像使用类方法一样调用方法，内部调用单例。外部将DB视作扮演Manager角色的实例，而不是类。
    // 使用_前缀，表示是DB类的私有方法
    static DB &_get_instance() {
        static DB instance;
        return instance;
    }

  public:
    // 构造单例时就初始化，且强制执行初始化，提高鲁棒性（耐操性）
    DB() {
        // 检查并创建database目录
        if (!std::filesystem::exists(dir + "database"))
            std::filesystem::create_directory(dir + "database");
        // 检查并创建database/books.db文件
        if (!std::filesystem::exists(dir + "database/books.db")) {
            std::ofstream(dir + "database/books.db");
            string books_db_path = dir + "database/books.db";
            books_db_ptr = make_shared<Database>(books_db_path, OPEN_READWRITE | OPEN_CREATE);
            books_db_ptr->exec(CREATE_TABLE_BOOKS);
        } else {
            books_db_ptr = make_shared<Database>(dir + "database/books.db", OPEN_READWRITE);
        }
        // 检查并创建database/users.db文件
        if (!std::filesystem::exists(dir + "database/users.db")) {
            std::ofstream(dir + "database/users.db");
            string users_db_path = dir + "database/users.db";
            users_db_ptr = make_shared<Database>(users_db_path, OPEN_READWRITE | OPEN_CREATE);
            users_db_ptr->exec(CREATE_TABLE_USERS);
        } else {
            users_db_ptr = make_shared<Database>(dir + "database/users.db", OPEN_READWRITE);
        }
        // 检查并创建database/admins.db文件
        if (!std::filesystem::exists(dir + "database/admins.db")) {
            std::ofstream(dir + "database/admins.db");
            string admins_db_path = dir + "database/admins.db";
            admins_db_ptr = make_shared<Database>(admins_db_path, OPEN_READWRITE | OPEN_CREATE);
            admins_db_ptr->exec(CREATE_TABLE_ADMINS);
        } else {
            admins_db_ptr = make_shared<Database>(dir + "database/admins.db", OPEN_READWRITE);
        }
        // 检查并创建database/borrow_records.db文件
        if (!std::filesystem::exists(dir + "database/borrow_records.db")) {
            std::ofstream(dir + "database/borrow_records.db");
            string borrow_records_db_path = dir + "database/borrow_records.db";
            borrow_records_db_ptr = make_shared<Database>(borrow_records_db_path, OPEN_READWRITE | OPEN_CREATE);
            borrow_records_db_ptr->exec(CREATE_TABLE_BORROW_RECORDS);
        } else {
            borrow_records_db_ptr = make_shared<Database>(dir + "database/borrow_records.db", OPEN_READWRITE);
        }
    }

    // 返回的是引用，节省内存。
    static Database &get_books_db() {
        return *DB::_get_instance().books_db_ptr;
    }

    static Database &get_users_db() {
        return *DB::_get_instance().users_db_ptr;
    }

    static Database &get_admins_db() {
        return *DB::_get_instance().admins_db_ptr;
    }

    static Database &get_borrow_records_db() {
        return *DB::_get_instance().borrow_records_db_ptr;
    }
};