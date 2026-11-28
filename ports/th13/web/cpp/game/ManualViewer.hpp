#pragma once

#include "AnmManager.hpp"

#include <array>
#include <functional>
#include <string>

namespace th13 {

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
    explicit ManualViewer(AnmManager& manager, i32 help_bank = 19) : animations(manager), bank(help_bank) {}
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

}
