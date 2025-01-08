#pragma once
#include "../TUI_utils.hpp"

using namespace ftxui;

class UserPage {
  public:
    UserPage() = delete;

    static void show_main_page(string account) {
        auto screen = ScreenInteractive::Fullscreen();
        // tab页面汇总
        vector<string> tab_titles = {"排行榜", "在借记录", "湖机儿", "湖搜", "账号管理", "菜单"};

        // 排行榜-------------------------------------------------------------------------------------------------------------
        int left_size = 120;
        auto left = Renderer([&] {
            return vbox({para("借阅排行榜", Color::White) |
                             bgcolor(LinearGradient().Angle(45).Stop(Color::DeepPink1).Stop(Color::DeepSkyBlue1)) |
                             bold | center,
                         para("排名     书名                           作者                               出版社       "
                              "      累计借阅次数",
                              Color::White),
                         separator(), para(Book::get_borrow_rank(), Color::White)});
        });
        auto right = Renderer([&] {
            return vbox({para("新出版排行榜", Color::White) |
                             bgcolor(LinearGradient().Angle(45).Stop(Color::DeepPink1).Stop(Color::DeepSkyBlue1)) |
                             bold | center,
                         para("排名     书名                           作者                               出版社       "
                              "              出版时间",
                              Color::White),
                         separator(), para(Book::get_new_publish_rank(), Color::White)});
        });
        auto split = ResizableSplitLeft(left, right, &left_size);
        Component tab_content_1 = split;

        // 在借记录------------------------------------------------------------------------------------------------------------------------
        Component tab_content_2 = Renderer([&] {
            return vbox({para("借阅记录", Color::White) | bold | center,
                         para("借阅上限：" + to_string(User::get_user_borrow_limit(account)), Color::White),
                         separator(), para(User::get_borrow_records(account), Color::White)});
        });

        // 湖机儿-------------------------------------------------------------------------------------------------------------
        string input_prompt = "", response = "";
        Component send_button = Button("询问湖机儿", [&]() {
            try {
                response = get_response(input_prompt);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        Component response_renderer = Renderer(
            [&] { return vbox({para("湖机儿说：", Color::White) | bold | center, para(response, Color::White)}); });
        auto tab_content_3 =
            Container::Vertical({Input(&input_prompt, "有什么问你湖？"), send_button, response_renderer});

        // 湖搜-------------------------------------------------------------------------------------------------------------
        int mode_selected = 0;
        string input_search_query_1 = "", input_search_query_2 = "";
        vector<string> modes = {"书名", "作者", "ISXN"};
        vector<string> mode_titles_to_show = {"按书名搜索", "按作者搜索", "按ISBN/ISSN搜索"};
        Component search_toggle = Toggle(&mode_titles_to_show, &mode_selected);
        Component search_button = Button("搜索", [&]() {
            try {
                vector<Component> entries = return_search(input_search_query_1, modes[mode_selected]);
                show_search_page("正在" + mode_titles_to_show[mode_selected], entries);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        Component author_books_button = Button("搜索", [&]() {
            try {
                show_search_page("作者书籍", return_author_books(input_search_query_2));
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        Component tab_content_4 = Container::Vertical(
            {Input(&input_search_query_1, "请输入搜索内容，支持输入片段来搜索，多个片段用空格隔开") | border,
             search_toggle | border, search_button,
             Renderer([&] { return vbox(filler() | size(HEIGHT, EQUAL, 10), separator()); }),
             Input(&input_search_query_2, "想看看作者有什么新书？搜搜看吧。请输入作者全名") | border,
             author_books_button});

        // 账号管理-------------------------------------------------------------------------------------------------------------
        string input_change_pwd_1 = "", input_change_pwd_2 = "";
        Component change_pwd_button = Button("更改密码", [&]() {
            if (input_change_pwd_1 == input_change_pwd_2) {
                User::change_pwd(account, input_change_pwd_1);
                show_tmp_page("密码更改成功", Color::Green);
            } else {
                show_tmp_page("两次输入的密码不一致", Color::Red);
            }
        });
        Component tab_content_5 =
            Container::Vertical({Input(&input_change_pwd_1, "请输入新密码") | border,
                                 Input(&input_change_pwd_2, "请再次输入新密码") | border, change_pwd_button});

        // 菜单-------------------------------------------------------------------------------------------------------------
        Component tab_content_6 = Container::Vertical({Button("退出", [&]() { screen.Exit(); }) | center});

        // 根据tab_selected的值，显示不同的内容。
        int tab_selected = 0;
        Component toggle = Toggle(&tab_titles, &tab_selected);
        Component tab_container = Container::Tab(
            {
                tab_content_1,
                tab_content_2,
                tab_content_3,
                tab_content_4,
                tab_content_5,
                tab_content_6,
            },
            &tab_selected);

        Component main_container = Container::Vertical({toggle, tab_container});
        Component main_renderer = Renderer(main_container, [&] {
            return vbox({
                       text("用户页面") | bold | center,
                       separator(),
                       toggle->Render(),
                       separator(),
                       tab_container->Render() | flex,
                   }) |
                   border;
        });
        screen.Loop(main_renderer);
    }
};
