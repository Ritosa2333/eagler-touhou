#pragma once
#include "BattleRenderOrder.hpp"
#include "BattleOverlay.hpp"
#include "BattlePortraits.hpp"
#include "BackgroundStage.hpp"
namespace th075 {
struct BattleArtwork {
    SpriteResources& background;SpriteResources& hud;SpriteResources& effects;
    std::array<SpriteResources*,2> fighters{},portraits{};
    std::array<std::vector<u32>,2> fighterTextures;std::array<std::array<u32,4>,2> captions{};
};
struct BattleTextDrawing {virtual ~BattleTextDrawing()=default;virtual void render()=0;};
class BattleRenderer {
    class Commands;ZunGraphics& graphics;BattleArtwork& art;BattleTextDrawing& text;
    u32 fpsStartedAt=0,fpsCount=0,fpsValue=0;u32 now=0;bool displayStage=true;
public:
    BackgroundStage background;BattleHud hud;BattleOverlay overlay;
    bool recoveryMarkers=false,disableTrails=false;
    BattleRenderer(ZunGraphics& graphics,BattleArtwork& art,BattleTextDrawing& text):graphics(graphics),art(art),text(text){}
    void render(BattleScene&,bool displayStage,u32 milliseconds);
    void combo(u8 side,i16 hits,i16 damage,i32 scale){if(side<2)hud.combos[side].update(hits,damage,i16(scale));}
    void begin_overlay(bool knockout,BattleProgress& progress){overlay.begin(knockout);progress.overlayTime=overlay.timer;}
    void advance_overlay(BattleProgress& progress){overlay.advance();progress.overlayTime=overlay.timer;}
};
}
