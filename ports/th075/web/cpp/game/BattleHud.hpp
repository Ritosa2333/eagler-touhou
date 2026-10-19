#pragma once
#include "BattleScene.hpp"
#include "ImageRenderer.hpp"
namespace th075 {
struct ComboCounter {
    i16 hits=0,damage=0,rate=0;u8 alpha=0;i8 facing=1;i16 wait=0,slideAngle=0;float slide=-200;
    void update(i16 hitCount,i16 totalDamage,i16 scale);
};
struct BattleHudView {
    std::array<FighterActor*,2> fighters{};GameMode mode=GameMode::Versus;BattlePhase phase=BattlePhase::Intro;
    u8 replayMode=0,pauseFade=0;bool paused=false,continued=false;i8 pauseChoice=0,continueCredits=0,remainingLives=0;
    float slide=0;std::array<u8,2> healthFade{},hudAlpha{255,255};u32 highScore=0;
    static BattleHudView from(BattleScene& scene){BattleHudView view;view.fighters={&scene.simulation.characters[0]->actor,&scene.simulation.characters[1]->actor};view.mode=scene.session.mode;view.phase=scene.simulation.rules.phase;view.replayMode=scene.session.replayMode;view.pauseFade=scene.progress.pauseFade;view.paused=scene.progress.paused;view.continued=scene.session.continued;view.pauseChoice=scene.progress.pauseChoice;view.continueCredits=scene.session.continueCredits;view.remainingLives=scene.simulation.rules.remainingLives;view.slide=scene.progress.victorySlide;view.healthFade=scene.progress.healthFade;view.hudAlpha=scene.simulation.story.hudAlpha;view.highScore=scene.simulation.rules.highScore;return view;}
};
class BattleHud {
public:
    std::array<ComboCounter,2> combos{};
    i32 bonusNumber=1,bannerTimer=0;
    void reset(){combos={};combos[0].facing=1;combos[1].facing=-1;bonusNumber=1;bannerTimer=0;}
    void number(ImageRenderer&,float x,float y,u32 color,i32 value,u8 style=4,i32 digits=0);
    void render_combo(ImageRenderer&,ComboCounter&);
    void render_bonus_banner(ImageRenderer&);
    void render_spells(ImageRenderer&,ImageRenderer& firstImages,ImageRenderer& secondImages,const BattleHudView&,const RoundRules&,u8 difficulty);
    void render_status(ImageRenderer&,ImageRenderer& firstImages,ImageRenderer& secondImages,const BattleHudView&);
    void render_pause_menu(ImageRenderer&,ImageRenderer& firstImages,ImageRenderer& secondImages,const BattleHudView&);
    void render_training_menu(ImageRenderer&,ImageRenderer& firstImages,const BattleHudView&);
};
}
