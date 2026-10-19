#include "BattleRenderOrder.hpp"
namespace th075 {
BattleDrawingState BattleDrawingState::from(BattleScene& scene,bool displayStage,bool markers){
    BattleDrawingState v;auto& sim=scene.simulation;v.cameraSpan=sim.story.camera.span;v.sceneFade=sim.rules.sceneFade;v.stageEnabled=displayStage;v.recoveryMarkers=markers;v.foreground=scene.foreground;v.fadeAlpha=sim.story.fadeAlpha;v.fadeColor=sim.story.fadeColor;v.phase=sim.rules.phase;
    for(u32 side=0;side<2;side++){const auto& actor=sim.characters[side]->actor;v.fighterFade[side]=actor.combat.fade;v.portrait[side]=actor.noticePortrait;}
    v.firstPortraitFront=sim.characters[0]->actor.notice.visible!=0;v.dialogueVisible=sim.characters[0]->actor.notice.dialogueVisible!=0;
    v.overlay=scene.progress.overlayTime>0;v.bonus=sim.rules.bonusTimer>0;v.pause=scene.progress.pauseFade!=0;return v;
}
void render_battle_layers(ZunGraphics& g,const BattleDrawingState& v,BattleDrawCommands& draw){
    const double span=v.cameraSpan;g.drawScale=float(1.0-((320.0-span)/320.0)/3.0);g.originX=float((320.0-span)*0.5-160.0);g.originY=float(((((320.0-span)*0.0)/320.0)*280.0)/2.0+((span/320.0)*-240.0-60.0));
    if(v.stageEnabled){
        g.colorMultiplier=v.sceneFade;draw.background_back();g.depth(DepthMode::Disabled);g.depth(DepthMode::WriteDisabled);
        draw.object_shadows(0);draw.object_shadows(1);draw.fighter_shadow(0);draw.fighter_shadow(1);
        g.colorMultiplier=1;draw.background_middle();g.depth(DepthMode::Disabled);g.depth(DepthMode::WriteDisabled);
        draw.owned_objects(0,2);draw.owned_objects(1,2);g.depth(DepthMode::Enabled);g.depth(DepthMode::WriteEnabled);
        draw.trails(0);draw.trails(1);draw.shared_effects(0);draw.owned_objects(0,0);draw.owned_objects(1,0);
        const u8 first=v.foreground?0:1,second=1-first;draw.fighter(first);draw.fighter(second);draw.character_special(first);draw.character_special(second);
        draw.owned_objects(0,1);draw.owned_objects(1,1);draw.shared_effects(1);g.colorMultiplier=v.sceneFade;draw.background_front();g.colorMultiplier=1;g.depth(DepthMode::Disabled);
        if(v.recoveryMarkers)draw.recovery_markers();
    }
    g.drawScale=1;g.originX=g.originY=0;
    if(v.fadeAlpha){g.blend(SpriteBlend::Alpha);g.depth(DepthMode::WriteEnabled);g.filled_rectangle({0,0,640,480},u32(v.fadeAlpha)<<24|v.fadeColor);}
    draw.status();draw.owned_objects(0,3);draw.owned_objects(1,3);
    for(u8 side=0;side<2;side++)if(v.fighterFade[side])draw.fade_portrait(side);draw.spells();
    if(v.phase==BattlePhase::VictoryText)draw.victory();
    else if(v.phase==BattlePhase::StoryDialogue){
        const u8 first=v.firstPortraitFront?1:0,second=1-first;if(v.portrait[first]>=0)draw.story_portrait(first);if(v.portrait[second]>=0)draw.story_portrait(second);
        if(v.dialogueVisible)draw.dialogue();draw.story_caption();
    }
    if(v.overlay)draw.battle_overlay();if(v.bonus)draw.bonus_banner();if(v.pause)draw.pause_menu();draw.fps();
}
}
