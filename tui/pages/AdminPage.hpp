#pragma once
#include "../TUI_utils.hpp"

class AdminPage {
  private:
    static string account;

  public:
    AdminPage() = delete;

    static void show_main_page(string account) {
        // 管理员账号
        AdminPage::account = account;

        // 借阅与归还-------------------------------------------------------------------------------------------------------------
        string user_account = "", isxn = "";
        auto borrow_button = Button("借阅", [&]() {
            try {
                Admin::borrow_book(user_account, isxn);
                show_tmp_page("借阅成功", Color::Green);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        auto return_button = Button("归还", [&]() {
            try {
                Admin::return_book(user_account, isxn);
                show_tmp_page("归还成功", Color::Green);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        auto tab_content_1 =
            Container::Vertical({Input(&user_account, "用户账号") | border, Input(&isxn, "输入ISBN/ISSN") | border,
                                 borrow_button, return_button});

        // 管理-------------------------------------------------------------------------------------------------------------
        auto user_management_button = Button("用户管理", [&]() { show_user_management_page(); });
        auto book_management_button = Button("图书管理", [&]() { show_book_management_page(); });
        auto tab_content_2 = Container::Vertical({user_management_button, book_management_button});

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
        Component tab_content_3 = split;

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
        auto tab_content_4 =
            Container::Vertical({Input(&input_prompt, "有什么问你湖？"), send_button, response_renderer});

        // 菜单-------------------------------------------------------------------------------------------------------------
        auto tab_content_5 = Container::Vertical({Button("退出", [] { throw string("exit"); }) | center});

        // 根据tab_selected的值，显示不同的内容。
        auto screen = ScreenInteractive::Fullscreen();
        int tab_selected = 0;
        vector<string> tab_titles = {"借阅与归还", "管理", "排行榜", "湖机儿", "菜单"};
        Component toggle = Toggle(&tab_titles, &tab_selected);
        Component tab_container = Container::Tab(
            {
                tab_content_1,
                tab_content_2,
                tab_content_3,
                tab_content_4,
                tab_content_5,
            },
            &tab_selected);
        Component main_container = Container::Vertical({toggle, tab_container});
        Component main_renderer = Renderer(main_container, [&] {
            return vbox({
                       text("管理员页面") | bold | center,
                       separator(),
                       toggle->Render(),
                       separator(),
                       tab_container->Render() | flex,
                   }) |
                   border;
        });
        screen.Loop(main_renderer);
    }

    // 用户管理-------------------------------------------------------------------------------------------------------------------------------------------------------
    static void show_user_management_page() {
        auto screen = ScreenInteractive::Fullscreen();
        string user_account = "";
        auto register_button = Button("注册用户", [&]() {
            try {
                Admin::register_user(user_account);
                show_tmp_page("注册成功", Color::Green);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        auto see_borrow_records_button = Button("查看借阅记录与上限", [&]() {
            try {
                string see = "借阅上限：" + to_string(User::get_user_borrow_limit(user_account)) + "\n" +
                             Admin::get_borrow_records(user_account);
                show_longtime_page(see, Color::CyanLight);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        auto reset_pwd_button = Button("重置密码为123456", [&]() {
            try {
                Admin::reset_user_pwd(user_account);
                show_tmp_page("重置成功", Color::Green);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });

        auto rm_button = Button("删除", [&]() {
            try {
                Admin::rm_user(user_account);
                show_tmp_page("删除成功（呈上淡淡的告别色）", Color::BlueLight);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        string set_borrow_limit_input = "";
        Component set_borrow_limit_button = Button("设置借阅上限", [&]() {
            try {
                Admin::set_user_borrow_limit(user_account, stoi(set_borrow_limit_input));
                show_tmp_page("设置成功", Color::Green);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        Component set_borrow_limit =
            Container::Horizontal({Input(&set_borrow_limit_input, "请输入上限") | border, set_borrow_limit_button});
        auto container = Container::Vertical({Input(&user_account, "输入用户账号") | border, register_button,
                                              see_borrow_records_button, reset_pwd_button, rm_button, set_borrow_limit,
                                              Button("返回", [&]() { screen.Exit(); })});
        screen.Loop(container);
    }

    // 图书管理-------------------------------------------------------------------------------------------------------------------------------------------------------
    static void show_book_management_page() {
        auto screen = ScreenInteractive::Fullscreen();
        // 新增-------------------------------------------------------------------------------------------------------------
        string input_title_1 = "", input_author_1 = "", input_publisher_1 = "", input_ISXN_1 = "", input_price_1 = "",
               input_publication_date_1 = "", input_intro_1 = "", input_number_in_stock_1 = "";

        Component add_new_button = Button("新增", [&]() {
            try {
                double price = input_price_1 == "" ? -1 : stod(input_price_1);
                int number_in_stock = input_number_in_stock_1 != "" ? stoi(input_number_in_stock_1) : 1;
                Admin::add_new_book(input_title_1, input_author_1, input_publisher_1, input_ISXN_1, price,
                                    input_publication_date_1, input_intro_1, number_in_stock);
                show_tmp_page("新增成功", Color::Green);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });

        Component tab_content_1 = Container::Vertical(
            {Renderer([] { return para("非必要项可以留空", Color::White); }),
             Input(&input_ISXN_1, "请输入ISBN/ISSN（后续不可修改") | border,
             Input(&input_title_1, "请输入书名") | border, Input(&input_author_1, "请输入作者") | border,
             Input(&input_publisher_1, "请输入出版社") | border, Input(&input_price_1, "请输入价格") | border,
             Input(&input_publication_date_1, "请输入出版时间，格式：YYYY-MM-DD") | border,
             Input(&input_intro_1, "请输入介绍") | border, Input(&input_number_in_stock_1, "请输入在架数目") | border,
             add_new_button});

        // 修改-------------------------------------------------------------------------------------------------------------
        string input_title_2 = "", input_author_2 = "", input_publisher_2 = "", input_ISXN_2 = "", input_price_2 = "",
               input_publication_date_2 = "", input_intro_2 = "", input_borrowings_number_2 = "",
               input_number_in_stock_2 = "";
        Component update_button = Button("修改", [&]() {
            try {
                double price = input_price_2 == "" ? -1 : stod(input_price_2);
                int borrowings_number = input_borrowings_number_2 == "" ? -1 : stoi(input_borrowings_number_2);
                int number_in_stock = input_number_in_stock_2 == "" ? -1 : stoi(input_number_in_stock_2);

                Admin::update_book_info(input_ISXN_2, input_title_2, input_author_2, input_publisher_2, price,
                                        input_publication_date_2, input_intro_2, borrowings_number, number_in_stock);
                show_tmp_page("修改成功", Color::Green);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });

        Component tab_content_2 = Container::Vertical(
            {Renderer([] { return para("留空为不修改", Color::White); }),
             Input(&input_ISXN_2, "务必输入ISBN/ISSN") | border, Input(&input_title_2, "请输入书名") | border,
             Input(&input_author_2, "请输入作者") | border, Input(&input_publisher_2, "请输入出版社") | border,
             Input(&input_price_2, "请输入价格") | border,
             Input(&input_publication_date_2, "请输入出版时间，格式：YYYY-MM-DD") | border,
             Input(&input_intro_2, "请输入介绍") | border,
             Input(&input_borrowings_number_2, "请输入累计借阅次数") | border,
             Input(&input_number_in_stock_2, "请输入在架数目") | border, update_button});

        // 增减图书-------------------------------------------------------------------------------------------------------------
        string input_existing_isxn = "", input_number = "";
        Component add_existing_button = Button("新增", [&]() {
            try {
                Admin::add_existing_book(input_existing_isxn, stoi(input_number));
                show_tmp_page("新增成功", Color::Green);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        Component rm_button = Button("删除", [&]() {
            try {
                Admin::rm_book(input_existing_isxn, stoi(input_number));
                show_tmp_page("删除成功", Color::BlueLight);
            } catch (string s) {
                show_tmp_page(s, Color::Red);
            } catch (...) {
                show_tmp_page("输入不合法", Color::Red);
            }
        });
        Component tab_content_3 = Container::Vertical(
            {Renderer([] { return para("当在架数目<0时会删除这本书", Color::BlueLight); }),
             Input(&input_existing_isxn, "请输入ISBN/ISSN") | border,
             Input(&input_number, "请输入要增加或减少的数目") | border, add_existing_button, rm_button});

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
             search_toggle | border, search_button, Renderer([&] {
                 return vbox(filler() | size(HEIGHT, EQUAL, 10), separator());
             }),
             Input(&input_search_query_2, "想看看作者有什么新书？搜搜看吧。请输入作者全名") | border,
             author_books_button});

        // 菜单-------------------------------------------------------------------------------------------------------------
        Component tab_content_5 = Container::Vertical({Button("退出", [&]() { screen.Exit(); }) | center});

        // 根据tab_selected的值，显示不同的内容。
        int tab_selected = 0;
        vector<string> tab_titles = {"新增图书", "修改图书", "增减图书", "湖搜", "菜单"};
        Component toggle = Toggle(&tab_titles, &tab_selected);
        Component tab_container = Container::Tab(
            {
                tab_content_1,
                tab_content_2,
                tab_content_3,
                tab_content_4,
                tab_content_5,
            },
            &tab_selected);

        Component main_container = Container::Vertical({toggle, tab_container});
        Component main_renderer = Renderer(main_container, [&] {
            return vbox({
                       text("图书管理页面") | bold | center,
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

string AdminPage::account = "";