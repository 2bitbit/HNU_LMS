#pragma once
#include "../models_utils.hpp"

class Book { // 所有书籍而非特定某个书籍，所以是静态成员函数
  public:
    // 获取书籍信息：通过ISBN/ISSN
    static string get_book_info(string isxn) {
        Statement query(DB::get_books_db(), "SELECT * FROM books WHERE ISXN = ?");
        // 防止SQL注入
        query.bind(1, isxn);
        if (query.executeStep()) {
            string price = query.getColumn(5).getInt() == -1
                               ? "暂无"
                               : to_string(round(query.getColumn(5).getDouble() * 100) / 100); // 保留两位小数
            return "书名: " + string(query.getColumn(1).getText()) + "\n" +
                   "作者: " + string(query.getColumn(2).getText()) + "\n" +
                   "出版社: " + string(query.getColumn(3).getText()) + "\n" +
                   "ISBN/ISSN: " + string(query.getColumn(4).getText()) + "\n" + "参考价格: " + price + "\n" +
                   "出版时间: " + string(query.getColumn(6).getText()) + "\n" +
                   "介绍: " + string(query.getColumn(7).getText()) + "\n" +
                   "累计借阅次数: " + to_string(query.getColumn(8).getInt()) + "\n" +
                   "在架数目: " + to_string(query.getColumn(9).getInt()) + "\n";
        } else {
            return "未找到相关书籍";
        }
    }

    // 查看借阅榜单前十五
    static string get_borrow_rank() {
        Statement query(DB::get_books_db(),
                        "SELECT 书名, 作者, 出版社, 累计借阅次数 FROM books ORDER BY 累计借阅次数 DESC LIMIT 15");
        int i = 1;
        string rank_str = "";
        while (query.executeStep()) {
            string rank = i < 10 ? to_string(i++) + " " : to_string(i++);
            string tmp_title =
                pad_str(string(query.getColumn(0).getText()) == "" ? "暂无" : string(query.getColumn(0).getText()), 35);
            string tmp_author =
                pad_str(string(query.getColumn(1).getText()) == "" ? "暂无" : string(query.getColumn(1).getText()), 35);
            string tmp_publisher =
                pad_str(string(query.getColumn(2).getText()) == "" ? "暂无" : string(query.getColumn(2).getText()), 30);
            string tmp_borrow_times = to_string(query.getColumn(3).getInt());
            rank_str +=
                rank + "  " + tmp_title + "  " + tmp_author + "  " + tmp_publisher + "  " + tmp_borrow_times + "\n";
        }
        return rank_str;
    }

    // 查看新出版排行榜前十五
    static string get_new_publish_rank() {
        Statement query(DB::get_books_db(),
                        "SELECT 书名, 作者, 出版社, 出版时间 FROM books ORDER BY 出版时间 DESC LIMIT 15");
        int i = 1;
        string rank_str = "";
        while (query.executeStep()) {
            string rank = i < 10 ? to_string(i++) + " " : to_string(i++);
            string tmp_title =
                pad_str(string(query.getColumn(0).getText()) == "" ? "暂无" : string(query.getColumn(0).getText()), 35);
            string tmp_author =
                pad_str(string(query.getColumn(1).getText()) == "" ? "暂无" : string(query.getColumn(1).getText()), 35);
            string tmp_publisher =
                pad_str(string(query.getColumn(2).getText()) == "" ? "暂无" : string(query.getColumn(2).getText()), 30);
            string tmp_publication_date =
                pad_str(string(query.getColumn(3).getText()) == "" ? "暂无" : string(query.getColumn(3).getText()), 10);
            rank_str +=
                rank + "  " + tmp_title + "  " + tmp_author + "  " + tmp_publisher + "  " + tmp_publication_date + "\n";
        }
        return rank_str;
    }

    // 湖搜服务
    // 湖搜的分词器
    static vector<string> _H_search_tokenizer(string query) {
        vector<string> res;
        // 正则表达式：匹配以任意空格分隔的字符串
        regex pattern(R"(\s+)");
        // 使用sregex_token_iterator进行分割
        sregex_token_iterator it(query.begin(), query.end(), pattern, -1);
        sregex_token_iterator end;

        while (it != end) {
            string token = *it++;
            if (!token.empty()) { // 忽略空字符串
                res.push_back(token);
            }
        }
        return res;
    }

    static vector<pair<string, string>> H_search(string query, string type) {
        // 输入验证
        if (type != "书名" && type != "作者" && type != "ISXN") {
            throw runtime_error("无效的搜索类型");
        }

        vector<string> tokens = Book::_H_search_tokenizer(query);
        if (tokens.empty()) {
            return {}; // 空查询返回空结果
        }

        // 构建SQL语句.参数化查询，防止SQL注入
        string sql = "SELECT 书名, 作者, 累计借阅次数, 出版社, 出版时间, ISXN FROM books WHERE "; // 最后一个必须是ISXN
        for (size_t i = 0; i < tokens.size(); ++i) {
            sql += type + " LIKE ? OR ";
        }
        sql = sql.substr(0, sql.size() - 3) + " ORDER BY 累计借阅次数 DESC";

        // 准备查询
        Statement db_query(DB::get_books_db(), sql);

        // 绑定参数
        for (int i = 0; i < tokens.size(); ++i) {
            db_query.bind(i + 1, "%" + tokens[i] + "%");
        }

        // 执行查询
        vector<pair<string, string>> res;
        while (db_query.executeStep()) {
            pair<string, string> row;
            string tmp = "";
            for (int i = 0; i < db_query.getColumnCount() - 1; ++i) {
                tmp += string(db_query.getColumn(i).getText()) + "  ";
            }
            row.first = tmp;
            row.second = string(db_query.getColumn(db_query.getColumnCount() - 1).getText());
            res.push_back(row);
        }
        return res;
    }

    // 按出版时间降序排列一个作者的 所有书籍
    static vector<pair<string, string>> get_author_books(string author) {
        if (!is_exist(author, "author")) {
            throw string("错误：不存在此作者：" + author);
        }
        Statement query(
            DB::get_books_db(),
            "SELECT 书名, 作者, 累计借阅次数, 出版社, 出版时间, ISXN FROM books WHERE 作者 = ? ORDER BY 出版时间 DESC");
        query.bind(1, author);
        vector<pair<string, string>> res;
        while (query.executeStep()) {
            pair<string, string> row;
            string tmp = "";
            for (int i = 0; i < query.getColumnCount() - 1; ++i) {
                tmp += string(query.getColumn(i).getText()) + "  ";
            }
            row.first = tmp;
            row.second = string(query.getColumn(query.getColumnCount() - 1).getText());
            res.push_back(row);
        }
        return res;
    }
};