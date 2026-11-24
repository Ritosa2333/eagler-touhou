#pragma once
#include "AnmManager.hpp"
#include "TitleState.hpp"
namespace th13 {
// 标题公共动画操作：只操作 title bank，bank 号由构造方传入（常驻 bank 表见 Application.cpp kBanks）。
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
}  // namespace th13
