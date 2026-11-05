#pragma once
#include "StorySequence.hpp"
#include "StoryDialogue.hpp"
namespace th075 {
struct BattleStoryPlatform:DialoguePlatform {
    virtual void reload_stage(u8)=0;virtual void stop_music()=0;virtual void sound(i16)=0;
};
// The character story callback and actual dialogue engine share the same actor
// state. Character events never duplicate or translate native object layouts.
class BattleStoryEvents final:public StoryEvents {
public:
    FighterActor& player;FighterActor& opponent;const FighterScript& script;
    const StoryEnvironment& environment;StoryDialogueCache& cache;BattleStoryPlatform& platform;bool automatic=false;
    BattleStoryEvents(FighterActor& player,FighterActor& opponent,const FighterScript& script,const StoryEnvironment& environment,StoryDialogueCache& cache,BattleStoryPlatform& platform,bool automatic):player(player),opponent(opponent),script(script),environment(environment),cache(cache),platform(platform),automatic(automatic){}
    void begin_dialogue(i16 label)override{begin_story_dialogue(player,label,environment.stage,platform);}
    void show_dialogue(bool visible)override{set_story_dialogue_visible(player,visible,environment.stage,platform);}
    bool advance_dialogue()override{return advance_story_dialogue(player,opponent,script,environment.stage,automatic,cache,platform);}
    void hide_portrait(FighterActor& a)override{a.notice.mode=0;}
    void show_portrait(FighterActor& a,u8 image,u8 direction)override{show_story_portrait(a,image,direction);}
    void advance_portrait(FighterActor& a)override{advance_story_portrait(a);}
    void reload_stage(u8 stage)override{platform.reload_stage(stage);}
    void select_music(i16 id)override{platform.select_music(id);}
    void start_music()override{platform.start_music();}
    void stop_music()override{platform.stop_music();}
    void sound(i16 id)override{platform.sound(id);}
    void replace_text(bool)override{platform.text("",120,120,255,16,1,false);}
};
}
