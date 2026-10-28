#pragma once
#include "TitleState.hpp"
#include "AnmManager.hpp"
namespace th13 {
// 标题公共动画操作（bank 号由 TitleSceneResources 传入；ascii 默认 5 是坑，
// th13 常驻 ascii=2，见 Application.cpp kBanks）。
class TitleAnimations {
    TitleState& state;AnmManager& animations;i32 title_bank,ascii_bank;
    bool check(bool);
public:
    std::string error;
    TitleAnimations(TitleState& s,AnmManager& a,i32 title=16,i32 ascii=5):state(s),animations(a),title_bank(title),ascii_bank(ascii){}
    bool create(i32 script);
    bool create_at(i32 script,const Vec3& translation);
    bool interrupt(i32 script,i32 label,bool immediate=false);
    bool handle_interrupt(u32 handle,i32 label,bool immediate=false);
    bool retire(i32 script);
    bool child_interrupt(i32 root,i32 script,i32 label,bool immediate=false);
    bool hide_child(i32 root,i32 script);
};
}
