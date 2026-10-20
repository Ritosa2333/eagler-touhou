#pragma once
#include "GameProfile.hpp"
#include "OptionsScene.hpp"
#include "ReplayFile.hpp"
#include "MenuDrawing.hpp"
namespace th075 {
struct NameEntrySignals:SceneSignals {virtual void render_name_entry()=0;virtual void present_name_entry()=0;virtual void save_score_name(u8)=0;virtual void save_replay_name()=0;};
class NameEntryScene {
public:
 bool replay=false,skip=false;i16 column=0,row=0,position=0,yesNo=0,action=0;u8 rank=0;
 void initialize(bool recording,i8 disposition=0){replay=recording;skip=recording&&disposition<0;column=row=position=yesNo=action=0;}
 void poll(const MenuInput&,const std::array<i32,2>& quickEnd,SceneSignals&);
 u16 advance(std::array<u8,8>& name,bool recording,i8 disposition,NameEntrySignals&);
 void render_score(ImageRenderer&,const std::array<ScoreRank,10>&,u32 now)const;
 void render_replay(ImageRenderer&,const ReplayFile&,const std::string& filename,u8 otherCharacter,u32 now)const;
};
}
