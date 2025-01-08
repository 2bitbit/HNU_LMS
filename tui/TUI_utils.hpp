#pragma once
#include "../utils.hpp"
using namespace ftxui;

// 工具函数：ftxui的paragraph中文无法换行，只好：遇见中文或中文标点就在其后面加一个空格。
// 同时进行修改，使得输入的\n会换行
Element para(string input, Color color_to_apply) {
    wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;
    u32string u32str = conv.from_bytes(input);
    u32string tmp;
    vector<Element> res;
    for (size_t i = 0; i < u32str.size(); ++i) {
        if (u32str[i] == U'\n') {
            res.push_back(paragraph(conv.to_bytes(tmp)) | color(color_to_apply));
            tmp = U"";
        } else {
            tmp += u32str[i];
            if (is_chinese_char(u32str[i])) {
                tmp += U' ';
            }
        }
    }
    res.push_back(paragraph(conv.to_bytes(tmp)) | color(color_to_apply));
    return vbox(res);
}

// 工具函数：渲染暂时的新页面
void show_tmp_page(string str_to_show, Color color_to_apply) {
    auto screen = ScreenInteractive::Fullscreen();
    // 3秒后返回
    thread([&]() {
        this_thread::sleep_for(chrono::seconds(3));
        screen.Exit();
    }).detach();
    auto renderer = Renderer(
        [&]() { return vbox(para(str_to_show, color_to_apply), paragraph("3 秒 后 返 回 ") | color(color_to_apply)); });
    screen.Loop(renderer);
};

// 工具函数：渲染要长时间显示的页面
void show_longtime_page(string str_to_show, Color color_to_apply) {
    auto screen = ScreenInteractive::Fullscreen();
    auto button = Button("返回", [&]() { screen.Exit(); });
    auto renderer = Renderer(button, [&]() { return vbox(para(str_to_show, color_to_apply), button->Render()); });
    screen.Loop(renderer);
}

// BFF--------------------------------------------------------------------------------------------------------------------------------------
// 处理模糊搜索
vector<Component> return_search(string query, string type) {
    vector<pair<string, string>> books = Book::H_search(query, type);
    if (books.empty()) {
        throw string("未找到相关书籍");
    }

    vector<Component> res;
    res.reserve(books.size()); // 预先分配空间，防止resize时频繁重新分配内存
    for (auto &book : books) {
        res.push_back(Container::Horizontal(
            {Renderer([=] { return para(book.first, Color::White); }) | flex, // 注意是[=]，而不是[&]
             Button("查看详情", [=] { show_longtime_page(Book::get_book_info(book.second), Color::White); }) |
                 align_right}));
    }

    return res;
}

// 工具函数：渲染有条目能跳转的搜索页面
void show_search_page(string title, vector<Component> items) {
    auto screen = ScreenInteractive::Fullscreen();
    int start_index = 0;
    vector<Component> showing_items;

    // 初始化显示
    for (int i = 0; i < min(start_index + 10, int(items.size())); i++) {
        showing_items.push_back(items[i]);
    }

    // 顶部框
    Component top_container = Container::Horizontal(
        {Renderer([&] { return para(title, Color::White); }) | flex, Button("返回", [&]() { screen.Exit(); })});
    Component top_renderer = Renderer(top_container, [&]() { return top_container->Render() | border; });

    // 主内容
    auto main_container = Container::Vertical(showing_items); // 初始化为首页内容
    auto main_renderer = Renderer(main_container, [&]() { return main_container->Render() | border; });

    // 更新显示内容的函数
    auto update_showing_items = [&]() {
        showing_items.clear();
        for (int i = start_index; i < min(start_index + 10, int(items.size())); i++) {
            showing_items.push_back(items[i]);
        }
        // 更新组件的正确方式！DetachAllChildren()后，main_container->Add(item)
        main_container->DetachAllChildren(); // 清空旧组件
        for (auto &item : showing_items) {
            main_container->Add(item); // 添加新组件
        }
    };

    // 底部页面前进后退按钮
    string jump_to_page = "";
    Component bottom_container =
        Container::Horizontal({Button("上一页",
                                      [&]() {
                                          if (start_index >= 10) {
                                              start_index -= 10;
                                              update_showing_items(); // 更新显示内容
                                          }
                                      }),
                               Button("下一页",
                                      [&]() {
                                          if (start_index + 10 < items.size()) {
                                              start_index += 10;
                                              update_showing_items(); // 更新显示内容
                                          }
                                      }),
                               Input(&jump_to_page, "或跳转至：") | align_right | border, Button("跳转", [&]() {
                                   if (!jump_to_page.empty()) {
                                       int p = stoi(jump_to_page);
                                       if (1 <= p && p <= 1 + (items.size() - 1) / 10) {
                                           start_index = (p - 1) * 10;
                                           update_showing_items(); // 更新显示内容
                                       }
                                   }
                               })});
    Component bottom_renderer = Renderer(bottom_container, [&]() { return bottom_container->Render() | border; });

    // 渲染
    Component page_container = Container::Vertical({top_renderer, main_renderer, bottom_renderer});
    Component page_renderer = Renderer(page_container, [&]() { return page_container->Render() | border; });
    screen.Loop(page_renderer);
}

// 处理作者书籍
// 每一个res元素是一个条目
vector<Component> return_author_books(string author) {
    vector<pair<string, string>> books = Book::get_author_books(author);
    if (books.empty()) {
        throw string("未找到相关书籍");
    }

    vector<Component> res;
    res.reserve(books.size()); // 预先分配空间，防止resize时频繁重新分配内存
    for (auto &book : books) {
        res.push_back(Container::Horizontal(
            {Renderer([=] { return para(book.first, Color::White); }) | flex,
             Button("查看详情", [=] { show_longtime_page(Book::get_book_info(book.second), Color::White); })}));
    }
    return res;
}