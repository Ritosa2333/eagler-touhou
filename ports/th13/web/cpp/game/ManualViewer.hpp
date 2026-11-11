#pragma once

#include "AnmManager.hpp"

#include <array>
#include <functional>
#include <string>

namespace th13 {

// 0x42d940/0x42d740/0x42da10 help 查看器：8 页菜单（help.anm script0..7）+
// 页图 VM（script9，entry1 的 blank texture）。宿主负责把 help_%.2d.png 解码
// 上传到该 blank 纹理；本类只驱动页序/翻页输入/结束位。
class ManualViewer {
    AnmManager& animations;
    i32 bank, page = 0, phase = 0, age = 0;
    bool done = false;
    std::array<u32, 8> entries{};
    u32 page_vm = 0;
    bool fail(const std::string&);
    bool menu();
    bool show_page();
public:
    std::string error;
    std::function<bool(i32)> load_page, sound;
    explicit ManualViewer(AnmManager& manager, i32 help_bank = 19)
        : animations(manager), bank(help_bank) {}
    ~ManualViewer();
    ManualViewer(const ManualViewer&) = delete;
    ManualViewer& operator=(const ManualViewer&) = delete;
    bool open();
    bool update(u32 pressed, u32 repeated);
    void close();
    bool finished() const { return done; }
    i32 current_page() const { return page; }
    bool reading_page() const { return phase == 2; }
};

} // namespace th13
