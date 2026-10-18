#pragma once
#include "TitleAnimations.hpp"
#include "Manual.hpp"
namespace th15 {
class TitleManualMenu {
 TitleState& state;TitleAnimations visuals;Manual& manual;
public:
 std::string error;TitleManualMenu(TitleState& s,AnmManager& a,Manual& manual,i32 title=16,i32 ascii=5):state(s),visuals(s,a,title,ascii),manual(manual){}
 bool update();
};
}
