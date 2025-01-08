// 闭包的对外接口
#pragma once
// 发挥目录结构的结构特点，来管理页面代码。由于页面的层次关系，只需要在tui.hpp中引入所有文件夹下的第一级页面即可。
// 由于ftxui的框架特性，采用函数式页面编程。并发挥函数栈的结构特点，来管理页面跳转。
#include "pages/AdminPage.hpp"
#include "pages/LoginPage.hpp"
#include "pages/UserPage.hpp"

// 显示页面
// 一个页面一个文件，一个页面一个函数，一个页面一个类。我选择：一个页面一个方法（成员函数）：
// 第一者目录结构的繁琐，第二者函数先后顺序安排对可读性的影响，第三者与框架API接口不合。
// 函数栈上没有 screen 在 loop 才能 exit(0)。
// 利用类的封装提高可扩展性：
//   -内存的特性，简化扩展时函数先后顺序的难题。从而可以按照更好的语义排布代码，提高可读性。（例如：采用普通函数是自底向上的代码顺序，采用类则可以用自顶向下的代码顺序提高可读性）
//   -利用类的私有成员，简洁实现类的跨函数的参数传递。
// 通过：
//   -在函数的末尾使用 loop，内部执行 screen.Exit()：来出一个栈。
//   -throw：来连续出栈直到之前任意一个有 try 的函数栈。
class TUI {
  public:
    static void show_tui() {
        try {
            LoginPage::show_main_page();
        } catch (vector<string> type_and_account) {
            if (type_and_account[0] == "admin") {
                TUI::switch_to_admin_page(type_and_account[1]);
            } else if (type_and_account[0] == "user") {
                TUI::switch_to_user_page(type_and_account[1]);
            }
        } catch (string s) {
            if (s == "exit") {
                exit(0);
            }
        }
    }

    static void switch_to_admin_page(string account) {
        // 管理员主页
        try {
            AdminPage::show_main_page(account);
        } catch (string s) {
            if (s == "exit") {
                exit(0);
            }
        }
    }

    static void switch_to_user_page(string account) {
        // 用户主页
        try {
            UserPage::show_main_page(account);
        } catch (string s) {
            if (s == "exit") {
                exit(0);
            }
        }
    }
};
