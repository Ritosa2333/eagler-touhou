#pragma once
#include "AnmManager.hpp"
#include "TitleState.hpp"
namespace th13 {
class TitleAnimations {
    TitleState& state;
    AnmManager& animations;
    i32 title_bank;
    bool check(bool);

public:
    std::string error;
    TitleAnimations(TitleState& s, AnmManager& a, i32 title = 16) : state(s), animations(a), title_bank(title) {}
    bool create(i32 script);
    bool create_at(i32 script, const Vec3& translation);
    bool interrupt(i32 script, i32 label, bool immediate = false);
    bool handle_interrupt(u32 handle, i32 label, bool immediate = false);
    bool retire(i32 script);
    bool child_interrupt(i32 root, i32 script, i32 label, bool immediate = false);
    bool hide_child(i32 root, i32 script);
};
}
