#pragma once
#include "TitleScene.hpp"
#include "ImageRenderer.hpp"
namespace th075 {
struct SelectionUnlocks {
    std::array<u8,256> storyFlags{};
    u8 character_available(i8 character,bool story)const;
    u8 background_available(i8 background)const;
};
struct SelectionSettings {std::array<u8,2> handicap{2,2},easyCommands{};};
struct SelectionProgress {
    std::array<std::array<u8,4>,11> storyStages{};u8 character=0,difficulty=0;
    void inspect(u8 player,u8 level){character=player;difficulty=level;}
    u8 stages()const{return storyStages[character][difficulty];}
};
struct SelectPlayer {
    u8 phase=0,character=0,previousCharacter=0,highlightAll=1,spell=0,cardsHidden=1,panel=0,panelCursor=0,help=0;
    i8 randomTicks=0;
    float portraitSlide=100,firstCardX=80,firstCardY=40,secondCardX=80,secondCardY=40,cardTurn=0,portraitOffset=100;
};
struct SelectInput {std::array<i32,7> counts{};};
struct CharacterSelectSignals:SceneSignals {
    virtual void render_selection()=0;virtual void present_selection()=0;virtual void prepare_battle()=0;
};
class CharacterSelectScene {
public:
    std::array<SelectPlayer,2> players{};std::array<SelectInput,2> inputs{};
    std::array<u8,15> duelUnlocks{},storyUnlocks{};std::array<u8,30> stageUnlocks{};
    std::array<i32,2> hardwareCancel{};i32 escapeHeld=0;u8 stageAlpha=0,backgroundVariant=0,backgroundCursor=0;
    float backgroundScroll=0,entryZoom=80,rotation=4.71238899230957f;
    void initialize(GameSelection&,SelectionProgress&,const SelectionUnlocks&);
    void reset(GameSelection&,SelectionProgress&);
    void poll(const std::array<SelectInput,2>&,bool escape,const GameSelection&);
    u16 advance(GameSelection&,SelectionSettings&,SelectionProgress&,ReplayRandom&,CharacterSelectSignals&);
    void render(ImageRenderer& images,ImageRenderer& characters,ImageRenderer& backgrounds,const GameSelection&,const SelectionSettings&,const SelectionProgress&)const;
    void render_spells(ImageRenderer& images,ImageRenderer& characters,ImageRenderer& backgrounds,const GameSelection&,const SelectionSettings&,const SelectionProgress&)const;
    void render_options(ImageRenderer& images,ImageRenderer& characters,ImageRenderer& backgrounds,const GameSelection&,const SelectionSettings&,const SelectionProgress&)const;
    void render_story_options(ImageRenderer& images,ImageRenderer& characters,ImageRenderer& backgrounds,const GameSelection&,const SelectionSettings&,const SelectionProgress&)const;
    void choose_character(u8 side,GameSelection&,SelectionSettings&,SelectionProgress&,ReplayRandom&,SceneSignals&);
    void choose_first_spell(u8 side,GameSelection&,SelectionSettings&,SelectionProgress&,ReplayRandom&,SceneSignals&);
    void choose_second_spell(u8 side,GameSelection&,SelectionSettings&,SelectionProgress&,ReplayRandom&,SceneSignals&);
    void choose_background(u8 side,GameSelection&,SelectionSettings&,SelectionProgress&,ReplayRandom&,SceneSignals&);
    void finish_selection(u8 side,GameSelection&,SelectionSettings&,SelectionProgress&,ReplayRandom&,SceneSignals&);
    void player_options(u8 side,GameSelection&,SelectionSettings&,SelectionProgress&,ReplayRandom&,SceneSignals&);
    void story_options(GameSelection&,SelectionSettings&,SelectionProgress&,ReplayRandom&,SceneSignals&);
};
}
