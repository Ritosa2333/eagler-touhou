#include "../game/CallbackNames.hpp"
#include "Hud.hpp"
#include "../game/TextFormat.hpp"
#include "../game/HighRefresh.hpp"
#include "AudioData.hpp"
#include <cmath>
#include <cstdlib>
namespace th10::browser {
HudMessages::HudMessages(Hud& h):owner(h){gui=h.actors.gui;game=&h.state.game;registry=&h.engine.manager.registry;keys=reinterpret_cast<const u32*>(&h.input.player_profiles[0].input.current);pressed=&h.input.player_profiles[0].input.pressed;rate=&h.engine.speed;decoded_text=text;}
AnmFile& HudMessages::file(DialogueAnimationFile kind){switch(kind){case DialogueAnimationFile::Player:return *owner.actors.player->animation_file;case DialogueAnimationFile::Boss:return *owner.actors.enemies->animation_files[2];case DialogueAnimationFile::Interface:return *gui->animations;case DialogueAnimationFile::Text:return *owner.common.value->text_animations;case DialogueAnimationFile::MusicCaption:return *gui->stage_animations;}__builtin_trap();}
Dialogue* HudMessages::allocate(){return static_cast<Dialogue*>(std::malloc(sizeof(Dialogue)));}
u32 HudMessages::create_animation(DialogueAnimationFile kind,i32 script){return owner.animation(file(kind),script);}
void HudMessages::bind_sprite(AnmVm& vm,DialogueAnimationFile kind,i32 sprite,bool explicit_file){(explicit_file?file(kind):*vm.animation_file).bind_sprite(vm,sprite);}
void HudMessages::draw_text(AnmVm* vm,u32 color,const char* pattern){char output[128];if(format_text(output,sizeof(output),pattern,nullptr,0)<0)__builtin_trap();AnmText::draw(*vm,color,output,TextAlignment::Left,owner.fonts);}
void HudMessages::clear_projectiles_and_enemies(){owner.actions.clear_for_dialogue();}
void HudMessages::play_sound(i32 id){owner.sound(id);}
void HudMessages::start_music(){owner.music().play(1,static_cast<u32>(reinterpret_cast<uintptr_t>(owner.state.current_stage->music)));}
void HudMessages::fade_music(float seconds){owner.music().fade(seconds);}
void HudMessages::complete_stage(){HudProgress env(owner);th10::complete_stage(env);}
HudFrame::HudFrame(Hud& h):owner(h){GuiFrameEnvironment::game=GuiDrawEnvironment::game=&h.state.game;player=&h.actors.player;GuiFrameEnvironment::enemies=GuiDrawEnvironment::enemies=&h.actors.enemies;spell_flags=&h.actors.spell->spell_flags;engine_flags=&h.state.engine_flags;pending_screen=&h.state.pending_screen;registry=&h.engine.manager.registry;}
void HudFrame::update_animation(AnmVm& vm){owner.engine.update(vm);}
void HudFrame::bind_digit(AnmFile& f,AnmVm& vm,i32 digit){f.bind_sprite(vm,digit);}
u32 HudFrame::create_animation(AnmFile& file,i32 script){return owner.animation(file,script);}
i32 HudFrame::update_dialogue(Dialogue& dialogue){HudMessages env(owner);return dialogue.tick(env);}
void HudFrame::release_dialogue(Dialogue* dialogue){std::free(dialogue);}
void HudFrame::play_sound(i32 id){owner.sound(id);}
void HudFrame::draw_animation(AnmVm& vm){owner.engine.draw(vm);}
void HudFrame::rectangle(const ScreenRect& rect,u32 color){const u32 colors[]={color,color,color,color};auto renderer=owner.engine.renderer();auto* manager=&owner.engine.manager;draw_screen_rectangle(rect,colors,&manager,renderer);}
float HudFrame::presentation_boss_health(float current){
    if(!high_refresh::active||!owner.boss_presentation_valid||!owner.actors.enemies||owner.actors.enemies->bosses[0]!=owner.previous_boss||std::abs(current-owner.previous_boss_health)>.1f)return current;
    return high_refresh::lerp_world(owner.previous_boss_health,current);
}
HudScore::HudScore(Hud& h):owner(h){static constexpr i32 normal[]={2000000,4000000,8000000,15000000,1000000000},extra[]={3000000,10000000,1000000000};game=&h.state.game;normal_extends=normal;extra_extends=extra;}
void HudScore::bind_digit(AnmFile& f,AnmVm& vm,i32 digit){f.bind_sprite(vm,digit);}
void HudScore::update_animation(AnmVm& vm){owner.engine.update(vm);}
void HudScore::add_life(){HudEconomy env(owner);
#ifdef TH_ENABLE_THPRAC
    env.practice=&owner.state.practice;
#endif
    game->add_lives(1,env);}
HudNotification::HudNotification(Hud& h):owner(h){registry=&h.engine.manager.registry;}
u32 HudNotification::create(AnmFile& file,i32 script){return owner.animation(file,script);}
void HudNotification::bind_sprite(AnmVm& vm,i32 sprite){vm.animation_file->bind_sprite(vm,sprite);}
void HudEconomy::show_notification(i32 script){auto& id=owner.actors.gui->power_notification;owner.engine.manager.registry.delete_and_clear(id);id=owner.animation(*owner.actors.gui->animations,script);}
void HudEconomy::play_global_sound(i32 id){owner.sound(id);}
void HudEconomy::update_lives(i32 lives){owner.actors.gui->update_lives(lives);}
HudProgress::HudProgress(Hud& h):owner(h){game=&h.state.game;gui=&h.actors.gui;statistics={reinterpret_cast<u8*>(h.records.data)};replay_mode=&h.state.replay->mode;stages=menu_data(h.state.chinese).stages;current_stage=&h.state.current_stage;all_clear_bonus=h.state.practice.all_clear_bonus;}
void HudProgress::stage_clear_notification(){owner.notify(6,0);}
void HudProgress::select_screen(i32 screen){owner.state.pending_screen=owner.state.engine_flags&0x1000?2:screen;}
void HudProgress::show_results(){owner.actions.show_clear_results();}
void HudProgress::fade_ending(){ScreenEffect::create(ScreenEffectKind::HideScreen,120,0,0,0,49,owner.screen_effects);}
Hud::Hud(GameState& s,GameActors& a,AnimationEngine& e,Common& c,Fonts& f,Input& i,Audio& sound,Scores& score,ScreenEffects& fx,HudActions& events):state(s),actors(a),engine(e),common(c),fonts(f),input(i),audio(sound),records(score),screen_effects(fx),actions(events),update_chain(&e.chain_value){
    game=&s.game;
#ifdef TH_ENABLE_THPRAC
    practice=&s.practice;
#endif
    current=&a.gui;stage=&s.current_stage;cached_message=&message_cache;rate=&e.speed;filename=resource_name;current_screen=&s.pending_screen;display_difficulty=&difficulty_visible;controller_stage=&a.session->replay_mode;effects=&c.value->effects;slots=e.manager.files;registry=&e.manager.registry;chain=&update_chain;callbacks=&e.callback_environment;update_callback=callback_id::HudUpdate;draw_callback=callback_id::HudDraw;e.register_receiver(*this);
}
Hud::~Hud(){shutdown();std::free(message_cache);engine.unregister_receiver(*this);}
bool Hud::initialize(){return actors.gui||GuiResources::create(*this);}
void Hud::activate(){GuiResources{*actors.gui,*this}.activate();}
void Hud::shutdown(){if(actors.gui){auto* gui=actors.gui;GuiResources{*gui,*this}.shutdown();std::free(gui);}}
#ifndef TH_NATIVE_PLATFORM
bool Hud::invoke(CallbackToken token,void* object,i32& result){if(token!=update_callback&&token!=draw_callback)return false;HudFrame env(*this);auto& gui=*static_cast<Gui*>(object);if(token==update_callback){previous_boss_health=gui.displayed_boss_health;previous_boss=actors.enemies?actors.enemies->bosses[0]:nullptr;boss_presentation_valid=true;result=gui.update(env);}else result=gui.draw(env);return true;}
#endif
void Hud::start_dialogue(i32 id){HudMessages env(*this);Dialogue::start(id,env);}
void Hud::notify(i32 kind,i32 value){HudNotification env(*this);actors.gui->notify(kind,value,env);}
void Hud::update_score(){HudScore env(*this);actors.gui->update_score(env);}
void Hud::update_power(i32 whole,i32 fraction){HudScore env(*this);actors.gui->update_power(whole,fraction,env);}
void Hud::sound(i32 id){audio.manager.queue_effect(id,0,sound_definitions);}
u32 Hud::animation(AnmFile& file,i32 script){return engine.manager.create(file,script,15,AnimationPlacement::WorldBack,engine,engine);}
AudioGame Hud::music(){AudioGame game{audio.manager,&records.data,&state.configuration.display_flags,&engine.speed};
#ifdef TH_ENABLE_THPRAC
    game.practice=&state.practice;
#endif
    return game;}
Gui* Hud::allocate(){return static_cast<Gui*>(std::malloc(sizeof(Gui)));}
AnmFile* Hud::load_animations(i32 slot,const char* name){return engine.manager.load(slot,name,engine.resources);}
void Hud::release_animations(AnmFile& file){file.release(engine.resources);}
void Hud::delete_object(void* object){std::free(object);}
void Hud::free_bytes(void* bytes){std::free(bytes);}
u8* Hud::read_file(const char* name){return ResourceFiles{records.files}.load(name,nullptr,false);}
void Hud::report_error(){error=-1;}
u32 Hud::create_animation(AnmFile& file,i32 script){return animation(file,script);}
void Hud::initialize_animation(AnmFile& file,AnmVm& vm,i32 script){file.initialize_script(vm,script,engine,engine.manager.started_scripts);}
void Hud::bind_sprite(AnmFile& file,AnmVm& vm,i32 sprite){file.bind_sprite(vm,sprite);}
}

namespace th10::browser {
void Hud::bind_callbacks(Callbacks& b){callback_context=this;
 b.bind(update_callback,this,[](void* p,void* o,i32){auto& h=*static_cast<Hud*>(p);auto& gui=*static_cast<Gui*>(o);h.previous_boss_health=gui.displayed_boss_health;h.previous_boss=h.actors.enemies?h.actors.enemies->bosses[0]:nullptr;h.boss_presentation_valid=true;HudFrame env(h);return gui.update(env);});
 b.bind(draw_callback,this,[](void* p,void* o,i32){HudFrame env(*static_cast<Hud*>(p));return static_cast<Gui*>(o)->draw(env);});
}
}
