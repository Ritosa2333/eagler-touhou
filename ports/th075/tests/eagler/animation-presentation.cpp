#include "BattleRenderer.hpp"
#include "ReimuCharacter.hpp"
#include <cassert>
#include <cstdio>
using namespace th075;
struct Story:StoryEvents{void begin_dialogue(i16)override{}void show_dialogue(bool)override{}bool advance_dialogue()override{return true;}void hide_portrait(FighterActor&)override{}void show_portrait(FighterActor&,u8,u8)override{}void advance_portrait(FighterActor&)override{}void reload_stage(u8)override{}void select_music(i16)override{}void start_music()override{}void stop_music()override{}void sound(i16)override{}void replace_text(bool)override{}};

struct Platform:BattleScenePlatform{
 void sound(i16)override{}void combo(u8,i16,i16,i32)override{}void spell_attempt(u8,u8)override{}void advance_stage()override{}
 void toggle_fullscreen()override{}void begin_round_recording()override{}void banner(bool)override{}
 void fade_music(i32,i32,u32,u32)override{}void stop_music(i32)override{}void finish_replay_round()override{}
 void dialogue_origin(float,float,u32)override{}void prepare_victory(RoundFighterState&)override{}
 void show_victory_line(const std::string&,u8)override{}void end_story_dialogue()override{}
 void render_before_story_load()override{}void release_battle_loader()override{}void stage_story_completed()override{}
 void reset_fighter(u32,RoundFighterState&)override{}void reset_stage_effects()override{}void advance_overlay()override{}
 void render_battle(BattleScene&,bool)override{}void present_battle()override{}
};
struct Graphics:GfxInterface{
 PipelineState state;PipelineState& pipeline()override{return state;}
 TextureSize texture_size(u32)override{return {16,16};}
 void draw(u32,Topology,const ScreenVertex*,u32)override{}void clear_depth()override{}
};
struct Empty:SpriteResources{SpriteImage image(i16)override{return {};}};
struct Sprites:SpriteResources{SpriteImage image(i16 id)override{return {u32(id)+1,16,16};}};
struct Text:BattleTextDrawing{void render()override{}};
int main(){
 FighterPattern pattern;pattern.sequences.resize(100);
 for(int action=0;action<100;++action){auto& s=pattern.sequences[action];s.present=true;s.frames.resize(16);for(int i=0;i<16;++i){s.frames[i].sprite=u16(action*16+i);s.frames[i].duration=2;s.frames[i].basicVisual=true;}}
 ReimuCharacter a,b;Platform platform;Story story;EffectPattern effects;BattleScene scene({&a,&b},effects,platform);
 for(int side=0;side<2;++side){auto& actor=scene.simulation.characters[side]->actor;actor.side=u8(side);actor.animation.pattern=&pattern;scene.simulation.reset_fighter(u8(side));actor.save_collision();}
 Graphics backend;ZunGraphics graphics(backend);Empty empty;Sprites sprites;Text text;
 BattleArtwork art{empty,empty,empty,{&sprites,&sprites},{&empty,&empty}};
 BattleRenderer renderer(graphics,art,text);scene.simulation.story.camera.span=320;
 int checks=0;
 const auto render=[&]{for(auto* c:scene.simulation.characters)c->actor.combat.noticeTimer=3;renderer.render(scene,true,0);for(auto* c:scene.simulation.characters){auto& actor=c->actor;assert(actor.animation.frame);assert(!actor.trails.entries.empty());assert(actor.trails.entries.back().sprite==i16(actor.animation.frame->sprite));++checks;}};
 // Story opening advances action 9 while contacts (and their old frames) are disabled.
 scene.simulation.mode=GameMode::Story;scene.simulation.rules.phase=BattlePhase::StoryDialogue;
 a.initialize_story(b.actor,scene.simulation.story);bool jumped=false,landed=false;
 for(int frame=0;frame<350;++frame){scene.simulation.update_inputs({},{});scene.simulation.advance(story);render();jumped|=a.actor.animation.cursor.action==9;landed|=jumped&&a.actor.animation.cursor.action==10;}
 assert(jumped&&landed);puts("PASS story entrance: jumping, landing and idle use live animation frames");
 // A stale attack snapshot must not pin either fighter during Spell Break recovery.
 scene.simulation.rules.phase=BattlePhase::StoryRecovery;
 for(auto* c:scene.simulation.characters){c->actor.select_action(50);c->actor.save_collision();c->actor.select_action(97);c->actor.transitionReady=false;c->actor.combat.spellPhase=0;c->actor.motion.y=0;}
 for(int frame=0;frame<45;++frame){scene.simulation.update_inputs({},{});scene.simulation.advance(story);render();}
 for(auto* c:scene.simulation.characters)assert(c->actor.animation.cursor.action==99);
 puts("PASS Spell Break: both fighters animate knockdown into the downed pose");
 // Paused/global-freeze presentations keep exactly the current pose, including frame zero.
 scene.simulation.rules.freezeFrames=60;
 for(int frame=0;frame<60;++frame){scene.simulation.advance(story);render();}
 for(auto* c:scene.simulation.characters)assert(c->actor.animation.cursor.action==99);
 printf("PASS frozen pose: %d rendered fighter frames checked\n",checks);
}
