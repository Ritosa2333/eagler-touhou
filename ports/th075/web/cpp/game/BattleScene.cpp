#include "BattleScene.hpp"
namespace th075 {
class BattleScene::Flow final:public BattleFlowSignals {
    BattleScene& scene;BattleScenePlatform& platform;
public:
    explicit Flow(BattleScene& scene):scene(scene),platform(scene.platform){}
    void begin_round_recording()override{platform.begin_round_recording();}
    void banner(bool knockout)override{platform.banner(knockout);}
    void fade_music(i32 target,i32 step,u32 interval,u32 completion)override{platform.fade_music(target,step,interval,completion);}
    void stop_music(i32 step)override{platform.stop_music(step);}
    void finish_replay_round()override{platform.finish_replay_round();}
    void dialogue_origin(float x,float y,u32 layout)override{platform.dialogue_origin(x,y,layout);}
    void prepare_victory(RoundFighterState& winner)override{platform.prepare_victory(winner);}
    void show_victory_line(const std::string& text,u8 alpha)override{platform.show_victory_line(text,alpha);}
    void end_story_dialogue()override{platform.end_story_dialogue();}
    void render_before_story_load()override{scene.render();}
    void release_battle_loader()override{platform.release_battle_loader();}
    void stage_story_completed()override{platform.stage_story_completed();}
    void reset_fighter(u32 side,RoundFighterState& fighter)override{scene.simulation.reset_fighter(u8(side));platform.reset_fighter(side,fighter);}
    void reset_stage_effects()override{scene.simulation.effects.clear();platform.reset_stage_effects();}
    void advance_overlay()override{platform.advance_overlay();}
};
class BattleScene::Frame final:public BattleFrameCallbacks {
    BattleScene& scene;const BattleSystemInput& input;StoryEvents& story;
public:
    Frame(BattleScene& scene,const BattleSystemInput& input,StoryEvents& story):scene(scene),input(input),story(story){}
    void poll_pause()override{scene.controls.poll_pause(input,scene.progress,scene.session.mode,scene.platform);}
    void render()override{scene.render();}
    void update_inputs()override{
        auto& battle=scene.simulation;battle.replayMode=scene.session.replayMode;
        battle.replay.mode=battle.replayMode==1?ReplayMode::Recording:battle.replayMode==2?ReplayMode::Playback:ReplayMode::Live;
        battle.update_inputs(input.players,input.cancel);
        for(u32 side=0;side<2;side++){for(const auto value:input.players[side].buttons)if(value!=0)scene.progress.demoFrames=3600;if(input.cancel[side]!=0)scene.progress.demoFrames=3600;}
    }
    void frame_events()override{auto& battle=scene.simulation;scene.controls.frame_events(input,battle.characters[0]->actor,battle.characters[1]->actor,scene.progress,scene.session,battle.replay,scene.timing.replayFrame);}
    void simulate()override{auto& b=scene.simulation;b.story.completedStages=u8(b.rules.remainingLives);b.story.route=u8(scene.session.continueCredits);const u8 lives=b.story.completedStages;b.advance(story);if(b.story.completedStages!=lives)b.rules.remainingLives=i8(b.story.completedStages);scene.session.continueCredits=i8(b.story.route);b.story.completedStages=u8(b.rules.remainingLives);}
    void pause_menu()override{auto& battle=scene.simulation;scene.controls.pause_menu(input,battle.characters[0]->actor,battle.characters[1]->actor,scene.progress,scene.session,scene.platform);}
    void present()override{scene.platform.present_battle();}
    BattleExit advance_state()override{
        auto& battle=scene.simulation;for(u32 side=0;side<2;side++)battle.rules.spellElapsed[side]=battle.characters[side]->actor.spellElapsed;
        scene.session.replayExhausted=battle.replay.exhausted;Flow flow(scene);
        const auto result=scene.progress.advance(battle.characters[0]->actor,battle.characters[1]->actor,scene.session,flow);
        for(u32 side=0;side<2;side++)battle.characters[side]->actor.spellElapsed=battle.rules.spellElapsed[side];return result;
    }
};
BattleScene::BattleScene(const std::array<CharacterController*,2>& fighters,const EffectPattern& effects,BattleScenePlatform& platform)
    :platform(platform),simulation(fighters,effects,platform),progress(simulation.rules){}
void BattleScene::initialize(const BattleSession& settings){
    session=settings;simulation.mode=session.mode;simulation.rules={};controls={};timing={};timing.replayMode=session.replayMode;
    progress.paused=false;progress.pauseFade=0;progress.pauseChoice=0;progress.trainingInstantRecovery=false;
    progress.dialogueProgress=progress.victoryLine=0;progress.overlayTime=progress.demoFrames=0;progress.victorySlide=0;progress.textSlide=-300;
    progress.healthFade.fill(0);progress.savedInputs={};
    if(session.mode==GameMode::Story){platform.dialogue_origin(35,360,1);simulation.characters[0]->actor.notice.dialogueVisible=0;progress.victorySlide=-200;simulation.rules.phase=BattlePhase::StoryDialogue;if(session.replayMode==2){session.replayMode=4;controls.recordingFastForward=true;}else if(session.replayMode==1)session.replayMode=3;}
    else simulation.story.camera.override_view(false,0,0,0);
}
BattleExit BattleScene::advance(const BattleSystemInput& input,StoryEvents& story){
    simulation.mode=session.mode;timing.replayMode=session.replayMode;Frame frame(*this,input,story);
    return advance_battle_frame(timing,progress,{&simulation.characters[0]->actor,&simulation.characters[1]->actor},frame);
}
void BattleScene::render(){
    auto& battle=simulation;
    if(!progress.paused&&progress.pauseFade==0){
        battle.story.camera.advance(battle.characters[0]->actor.motion,battle.characters[1]->actor.motion);
        if(battle.rules.sceneFade<1){if(battle.rules.sceneFade<0)battle.rules.sceneFade=0;battle.rules.sceneFade=float(double(battle.rules.sceneFade)+double(0.01f));if(battle.rules.sceneFade>1)battle.rules.sceneFade=1;}
        const auto a=battle.characters[0]->actor.animation.cursor.action,b=battle.characters[1]->actor.animation.cursor.action;
        if(b<300){if(a>=300)foreground=0;}else if(a<300)foreground=1;
    }
    platform.render_battle(*this,timing.draw);
}
}
