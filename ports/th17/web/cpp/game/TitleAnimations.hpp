#pragma once
#include "TitleState.hpp"
#include "AnmManager.hpp"
namespace th17 {
class TitleAnimations {
    TitleState& state;AnmManager& animations;i32 title_bank,ascii_bank;
    bool check(bool);
public:
    static constexpr i32 prompt_slot=290;
    std::string error;
    TitleAnimations(TitleState& s,AnmManager& a,i32 title=16,i32 ascii=0):state(s),animations(a),title_bank(title),ascii_bank(ascii){}
    bool present(i32 script);
    bool create(i32 script);bool prompt();bool retire(i32 script);
    bool interrupt(i32 script,i32 label,bool immediate=false);
    bool child_interrupt(i32 root,i32 script,i32 label,bool immediate=false);
    bool hide_child(i32 root,i32 script);
};
}
