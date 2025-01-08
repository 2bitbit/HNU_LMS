#pragma once
#include "../TUI_utils.hpp"

class LoginPage {
  private:
    // 不能因为退出管理员登录页面就重置密码错误次数。生命周期与程序一致，采用封装在类中的全局变量--静态成员变量。
    static int admin_wrong_pwd_cnt;
    static int key_wrong_cnt;

  public:
    // 只是实现封装，不实例化。
    LoginPage() = delete;

    // 登录初始页面✔️
    static void show_main_page() {
        // 首页渲染部分
        auto screen = ScreenInteractive::Fullscreen();
        string l1 = R"(  __    __    _____  ___    ____  ____   )";
        string l2 = R"( /" |  | "\  (\"   \|"  \  ("  _||_ " |  )";
        string l3 = R"((:  (__)  :) |.\\   \    | |   (  ) : |  )";
        string l4 = R"( \/      \/  |: \.   \\  | (:  |  | . )  )";
        string l5 = R"( //  __  \\  |.  \    \. |  \\ \__/ //   )";
        string l6 = R"((:  (  )  :) |    \    \ |  /\\ __ //\   )";
        string l7 = R"( \__|  |__/   \___|\____\) (__________)  )";
        string l8 = R"(                                         )";
        auto title = Renderer([&] {
                         return vbox({text(l1), text(l2), text(l3), text(l4), text(l5), text(l6), text(l7), text(l8),
                                      text("欢迎来到，H 大图书管理系统") | center});
                     }) |
                     center | flex | border | color(Color::Red1);
        Component user_login_button = Button("用户登录", [&]() { LoginPage::show_user_login_page(); });
        Component admin_login_button = Button("管理员登录", [&]() { LoginPage::show_admin_login_page(); });
        Component admin_register_button = Button("管理员注册", [&]() { LoginPage::show_admin_register_page(); });
        Component library_contact_button =
            Button("图书馆管理员联系方式", [&]() { LoginPage::show_library_contact_page(); });
        Component exit_button = Button("退出", [&]() { throw string("exit"); });

        // 容器与渲染器
        Component main_container = Container::Vertical(
            {title, user_login_button, admin_login_button, admin_register_button, library_contact_button, exit_button});
        Component main_renderer = Renderer(main_container, [&]() {
            return vbox(main_container->Render()) |
                   bgcolor(LinearGradient().Angle(45).Stop(Color::DeepPink1).Stop(Color::DeepSkyBlue1));
        });
        screen.Loop(main_renderer);
    }

    // 用户登录页面✔️
    static void show_user_login_page() {
        auto screen = ScreenInteractive::Fullscreen();
        string account = "";
        string password = "";
        Component input_account = Input(&account, "请输入用户账号") | border;
        Component input_password = Input(&password, "请输入用户密码") | border;

        Component login_button = Button("登录", [&]() {
            try {
                User::login_user(account, hash_pwd(password));
                throw vector<string>{"user", account};
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            }
        });
        Component back_button = Button("取消", [&]() { screen.Exit(); });

        // 容器与渲染器
        Component main_container = Container::Vertical({input_account, input_password, login_button, back_button});
        Component main_renderer = Renderer(main_container, [&] { return vbox(main_container->Render()); });
        screen.Loop(main_renderer);
    };

    // 管理员登录页面✔️
    static void show_admin_login_page() {
        auto screen = ScreenInteractive::Fullscreen();
        string account = "";
        string password = "";
        Component input_account = Input(&account, "请输入管理员账号") | border;
        Component input_password = Input(&password, "请输入管理员密码") | border;
        Component login_button = Button("登录", [&]() {
            try {
                Admin::login_admin(account, hash_pwd(password));
                throw vector<string>{"admin", account};
            } catch (string s) {
                show_tmp_page(s + "\n错误次数：" + to_string(++admin_wrong_pwd_cnt) + "/3", Color::Red);
                if (admin_wrong_pwd_cnt == 3) {
                    throw string("exit");
                }
            }
        });
        Component back_button = Button("取消", [&]() { screen.Exit(); });

        // 容器与渲染器
        Component main_container = Container::Vertical({input_account, input_password, login_button, back_button});
        Component main_renderer = Renderer(main_container, [&] {
            return vbox(para("管理员登陆页面\n错误3次自动退出", Color::White), main_container->Render());
        });
        screen.Loop(main_renderer);
    }

    // 管理员注册页面✔️
    static void show_admin_register_page() {
        auto screen = ScreenInteractive::Fullscreen();
        string account = "";
        string password = "";
        string product_key = "";
        string contact_info = "";
        Component input_account = Input(&account, "请输入管理员账号") | border;
        Component input_password = Input(&password, "请输入管理员密码") | border;
        Component input_product_key = Input(&product_key, "请输入产品密钥") | border;
        Component input_contact_info = Input(&contact_info, "请输入管理员联系方式") | border;

        Component register_button = Button("注册", [&]() {
            try {
                Admin::register_admin(account, hash_pwd(password), contact_info, product_key);
                show_tmp_page("注册成功", Color::Green);
                // 借助函数栈管理历史页面：这里使用screen.Exit()退出注册页面，返回上级页面
                screen.Exit();
            } catch (string s) {
                show_tmp_page(s + "\n错误次数：" + to_string(++key_wrong_cnt) + "/3", Color::Red);
                if (key_wrong_cnt == 3) {
                    throw string("exit");
                }
            }
        });
        Component back_button = Button("取消", [&]() { screen.Exit(); });

        // 容器与渲染器
        Component main_container = Container::Vertical(
            {input_account, input_password, input_product_key, input_contact_info, register_button, back_button});
        Component main_renderer = Renderer(main_container, [&]() { return vbox(main_container->Render()); });
        screen.Loop(main_renderer);
    }

    // 图书馆管理员联系方式页面✔️
    static void show_library_contact_page() {
        auto screen = ScreenInteractive::Fullscreen();
        Component content_renderer =
            Renderer([&]() { return para(Admin::get_admins_contact(), Color::CyanLight) | border; });
        Component back_button = Button("返回", [&]() { screen.Exit(); });

        // 容器与渲染器
        Component main_container = Container::Vertical({content_renderer, back_button});
        Component main_renderer = Renderer(main_container, [&]() { return vbox(main_container->Render()); });
        screen.Loop(main_renderer);
    }
};

int LoginPage::admin_wrong_pwd_cnt = 0;
int LoginPage::key_wrong_cnt = 0;
