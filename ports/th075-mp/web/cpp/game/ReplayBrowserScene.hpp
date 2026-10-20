#pragma once
#include "ReplayFile.hpp"
#include "OptionsScene.hpp"
#include "MenuDrawing.hpp"
namespace th075 {
struct ReplayListing {std::string filename;ReplayFile replay;u8 otherCharacter=0;};
struct ReplayBrowserSignals:SceneSignals {virtual void render_replays()=0;virtual void present_replays()=0;virtual void start_replay(u32 index,u8 round)=0;};
class ReplayBrowserScene {
public:
 std::vector<ReplayListing> entries;i16 selected=0,round=0;u8 alpha=5;bool roundSelection=false;
 void initialize(i16 previous){selected=entries.empty()?0:previous<0?0:previous>=i16(entries.size())?i16(entries.size()-1):previous;round=0;alpha=5;roundSelection=false;}
 u16 advance(const MenuInput&,ReplayBrowserSignals&);
 void render(ImageRenderer&)const;
};
}
