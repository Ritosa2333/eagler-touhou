#pragma once
#include <algorithm>
namespace th19 {
// Native 4e0050/4e0290: controller mode5 selects the character pair;
// mode6 wraps all39 entries, including ID0. This is selection, not inventory.
struct VsCardSelection {
 int index=0;
 void reset(int value=0){index=value;}
 int move(int delta,bool all){const int count=all?39:2;index=(index+delta%count+count)%count;return index;}
 int card(int character,bool all)const{return all?index:std::clamp(character,0,18)*2+1+index;}
 // ANM interrupts for selected, adjacent and departing thumbnails.
 static constexpr int right[4]={15,17,11,13};
 static constexpr int left[4]={16,18,12,14};
};
}
