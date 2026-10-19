#pragma once
#include "GameProfile.hpp"
#include "CardList.hpp"
#include "MenuDrawing.hpp"
#include "OptionsScene.hpp"
namespace th075 {
struct ResultListSignals:SceneSignals {
 virtual void render_result_list()=0;virtual void present_result_list()=0;
 virtual void load_result_character(u8)=0;virtual void update_result_text(bool)=0;
};
class ResultListScene {
public:
 i16 page=0;u8 alpha=5,character=0,difficulty=0;bool cardsMode=false;SelectionUnlocks unlocks;
 u16 advance(const MenuInput&,u32 cardCount,ResultListSignals&);
 std::string text(const GameProfile&,const CardList&)const;
 void render(ImageRenderer&,u32 textTexture,const GameProfile&,u32 cardCount)const;
};
}
