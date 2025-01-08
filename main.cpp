#include "models/models.hpp"
#include "tui/TUI.hpp"

// 主流现代语言几乎都不采用声明和实现分离的做法。且cpp正在推进模块化，故不采用声明和实现分离的做法。
int main() {
    TUI::show_tui();
    return 0;
}