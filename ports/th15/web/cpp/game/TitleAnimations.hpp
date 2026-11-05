#pragma once
#include "TitleState.hpp"
#include "AnmManager.hpp"
namespace th15 {
class TitleAnimations {
    TitleState& state;AnmManager& animations;i32 title_bank,ascii_bank;
    bool check(bool);
public:
    std::string error;
    TitleAnimations(TitleState& s,AnmManager& a,i32 title=16,i32 ascii=5):state(s),animations(a),title_bank(title),ascii_bank(ascii){}
    bool create(i32 script);
    bool prompt();bool retire(i32 script);
    bool interrupt(i32 script,i32 label,bool immediate=false);
    bool child_interrupt(i32 root,i32 script,i32 label,bool immediate=false);
    bool hide_child(i32 root,i32 script);
};
}
