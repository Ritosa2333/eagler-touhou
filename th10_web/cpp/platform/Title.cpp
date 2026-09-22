#include "../game/CallbackNames.hpp"
#include "Title.hpp"
#include "AudioData.hpp"
#include "../game/TextFormat.hpp"
#include <cstdlib>
#include <new>
namespace th10::browser {
template<class Base> MenuMain<Base>::MenuMain(Title& title):owner(title){
    owner.bind_animation(*this);this->game=&owner.state.game;
    this->extra_unlocked=reinterpret_cast<u8*>(owner.scores.data)+0x1d888;
    this->pressed=reinterpret_cast<u32*>(&owner.input.player_profiles[0].input.raw_pressed);
    this->repeated=&owner.input.player_profiles[0].input.raw_repeat;
}
template<class Base> u32 MenuMain<Base>::create(AnmFile& file,i32 script){return owner.create(file,script);}
template<class Base> void MenuMain<Base>::sound(i32 sound){owner.sound(sound);}
template<class Base> void MenuMain<Base>::interrupt_immediately(u32 id,i32 label){this->registry->interrupt_and_update(id,static_cast<std::int16_t>(label),owner.engine);}
template<class Base> MenuSelection<Base>::MenuSelection(Title& owner):MenuMain<Base>(owner){
    this->scores=&owner.scores.data;this->effects=&owner.common.value->effects;this->stage_table=owner.data.stages;this->current_stage=&owner.state.current_stage;
    this->remembered_stage=&owner.state.remembered_stage;this->practice_shortcut=&owner.state.practice_shortcut;this->pending_screen=&owner.state.pending_screen;this->keyboard=owner.state.keyboard;
}
template<class Base> void MenuSelection<Base>::show_loading(float x,float y){auto& o=this->owner;auto& id=o.common.value->loading_animation;if(!id)id=o.engine.manager.create_at(**this->effects,6,{x,y,0},false,AnimationPlacement::WorldBack,o.engine,o.engine);}
template<class Base> void MenuSelection<Base>::hide_screen(){ScreenEffect::create(ScreenEffectKind::HideScreen,32,0,0,0,43,this->owner.effects);}
template<class Base> void MenuSelection<Base>::fade_music(float seconds){this->owner.music_control().fade(seconds);}
template<class Base> bool MenuSelection<Base>::read_keyboard(){return InputDevices{this->owner.input}.read_keyboard(this->keyboard);}
MenuKeys::MenuKeys(Title& o):MenuMain(o){active_bindings=reinterpret_cast<u16*>(o.input.player_profiles[0].bindings);saved_bindings=o.state.configuration.keys;}
const u8* MenuKeys::buttons(){return InputDevices{owner.input}.controller_buttons(0);}
void MenuKeys::bind_digit(AnmVm& vm,i32 sprite){vm.animation_file->bind_sprite(vm,sprite);}
MenuOptions::MenuOptions(Title& o):MenuMain(o){settings=reinterpret_cast<TitleAudioSettings*>(&o.state.configuration.music_volume);music_volume=&o.audio.manager.music_volume;effects_volume=&o.audio.manager.effects_volume;effects_gain=&o.audio.manager.effects_gain;}
void MenuOptions::apply_music_volume(){owner.audio.manager.queue_music(8,0,"SetVol");}
void MenuOptions::bind_digit(AnmVm& vm,i32 sprite){vm.animation_file->bind_sprite(vm,sprite);}
MenuScores::MenuScores(Title& o):MenuMain(o){
    scores=&o.scores.data;effects=&o.common.value->effects;rows=&o.common.value->text_animations;spell_difficulties=o.scores.spell_difficulties;unknown_spell_format=o.data.unknown_spell;unlock_sequence=o.data.unlock_sequence;
    unlock_cursor=&o.state.unlock_cursor;unlock_elapsed=&o.state.unlock_elapsed;keyboard=o.state.keyboard;previous_keyboard=o.state.previous_keyboard;pressed_keyboard=o.state.pressed_keyboard;
}
bool MenuScores::read_keyboard(){return InputDevices{owner.input}.read_keyboard(keyboard);}
void MenuScores::text(AnmVm* vm,u32 color,const char* pattern,const u32* args,u32 count){owner.text(*vm,color,pattern,args,count,TextAlignment::Center);}
MenuReplays::MenuReplays(Title& o):MenuSelection(o){remembered_replay=&o.state.remembered_replay;return_screen=&o.state.return_screen;replay_filename=o.state.replay_filename;}
Replay* MenuReplays::preview(const char* file){return owner.preview(file);}
void MenuReplays::delete_replay(Replay* replay){owner.delete_replay(replay);}
u32 MenuReplays::find_first(const char* value,ReplaySearchEntry& entry){if(std::strlen(value)>=sizeof(pattern))return 0xffffffff;std::strcpy(pattern,value);search_index=0;searching=true;if(find_next(1,entry))return 1;searching=false;return 0xffffffff;}
bool MenuReplays::find_next(u32 id,ReplaySearchEntry& entry){if(id!=1||!searching)return false;std::memset(&entry,0,sizeof(entry));return owner.scores.files.host.list("replay",pattern,search_index++,entry.filename,sizeof(entry.filename))!=0;}
void MenuReplays::find_close(u32){searching=false;}
#ifdef TH_ENABLE_THPRAC
// THGuiRep State(1/2/3) for the title replay menu. The raw file is re-read here
// (upstream CheckReplay reads replay/<name> directly) because the decoded
// Replay object does not retain the trailing USER/PRAC area.
void MenuReplays::reset_practice(){practice_replay_menu_reset(owner.state.practice);}
void MenuReplays::check_practice(const char* filename){
    auto& files=owner.scores.files;const auto length=std::strlen(filename);if(length>255)return;
    char path[264];std::memcpy(path,"replay/",7);std::memcpy(path+7,filename,length+1);
    const auto handle=files.host.open(path,false);if(handle==0xffffffff)return;
    const auto size=files.host.size(handle);
    if(size>16*1024*1024){files.host.close(handle);return;}
    std::vector<u8> raw(size);const bool complete=files.host.read(handle,raw.data(),size)==size;files.host.close(handle);
    if(complete)practice_replay_menu_check(owner.state.practice,raw.data(),u32(size));
}
void MenuReplays::activate_practice(){practice_replay_menu_activate(owner.state.practice);}
#endif
MenuMusic::MenuMusic(Title& o):owner(o){
    o.bind_animation(*this);effects=&o.common.value->effects;comment_file=&o.engine.manager.files[0];unlocked=reinterpret_cast<u8*>(o.scores.data)+0x1d892;
    display_flags=&o.state.configuration.display_flags;pressed=o.main.pressed;repeated=o.main.repeated;locked_title=o.data.locked_title;locked_comments=o.data.locked_comments;
}
u32 MenuMusic::create(AnmFile& file,i32 script){return owner.create(file,script);}
char* MenuMusic::read_file(i32& length){return reinterpret_cast<char*>(ResourceFiles{owner.scores.files}.load("musiccmt.txt",reinterpret_cast<u32*>(&length),false));}
void MenuMusic::free_file(char* bytes){std::free(bytes);}
void MenuMusic::text(AnmVm& vm,u32 color,const char* text){owner.text(vm,color,text,nullptr,0,TextAlignment::Left);}
void MenuMusic::numbered_text(AnmVm& vm,u32 color,i32 track_number,const char* title){const u32 arguments[2]={static_cast<u32>(track_number),reinterpret_cast<uintptr_t>(title)};owner.text(vm,color,"No.%d  %s",arguments,2,TextAlignment::Left);}
void MenuMusic::locked_text(AnmVm& vm,u32 color,i32 track_number){const u32 argument=static_cast<u32>(track_number);owner.text(vm,color,locked_title,&argument,1,TextAlignment::Left);}
void MenuMusic::sound(i32 id){owner.sound(id);}
void MenuMusic::music_command(i32 command){owner.audio.manager.queue_music(command,0,"dummy");}
void MenuMusic::load_music(const char* file){owner.music_control().prepare(0,file);}
void MenuMusic::play_music(){owner.music_control().play(0,0);}
MenuDraw::MenuDraw(Title& o):owner(o){game=&o.state.game;scores=&o.scores.data;replay=&o.state.replay;alphabet=o.data.alphabet;characters=o.data.characters;difficulties=o.data.difficulties;stage_names=o.data.stage_names;replay_stage_names=o.data.replay_stage_names;color=&o.common.value->color;text_mode=reinterpret_cast<u32*>(&o.common.value->shadow);}
ReplayDate MenuDraw::local_date(i32 timestamp){return owner.calendar.local_date(timestamp);}
void MenuDraw::text(const Vec3& position,const char* pattern,const u32* args,u32 count){char text[512];if(format_text(text,sizeof(text),pattern,args,count)<0)__builtin_trap();owner.common.value->queue(text,position,false);}
MenuClear::MenuClear(Title& o):MenuMain(o){scores=&o.scores.data;replay=&o.state.replay;effects=&o.common.value->effects;stages=o.data.stages;current_stage=&o.state.current_stage;alphabet=o.data.alphabet;ranking=o.results;}
void MenuClear::play_music(bool result){owner.music_control().prepare(0,result?"bgm/th10_17.wav":"bgm/th10_02.wav");owner.music_control().play(0,result?17:0);}
void MenuClear::timestamp(i32& result){result=owner.calendar.timestamp();}
Replay* MenuClear::preview(const char* name){return owner.preview(name);}
void MenuClear::delete_replay(Replay* replay){owner.delete_replay(replay);}
void MenuClear::save_replay(const char* file,const char* player){if(!owner.state.replay)__builtin_trap();const auto& motion=owner.state.motion;const auto tail=motion.playing?std::vector<u8>{}:motion.trailer(10);if((motion.used()&&!motion.playing&&tail.empty())||owner.writer.save(*owner.state.replay,file,player,tail))__builtin_trap();}
MenuLoop::MenuLoop(Title& o):owner(o){
    o.bind_animation(*this);game=&o.state.game;return_screen=&o.state.return_screen;inactive_frames=&o.state.inactive_frames;demo_index=&o.state.demo_index;pending_screen=&o.state.pending_screen;
    held=reinterpret_cast<u32*>(&o.input.player_profiles[0].input.raw);engine_flags=&o.state.engine_flags;files=o.engine.manager.files;loading_animation=&o.common.value->loading_animation;stages=o.data.stages;current_stage=&o.state.current_stage;demo_files=o.data.demo_files;replay_filename=o.state.replay_filename;
}
u32 MenuLoop::create(AnmFile& file,i32 script){return owner.create(file,script);}
Replay* MenuLoop::load_replay(const char* name){return owner.preview(name);}
void MenuLoop::delete_replay(Replay* replay){owner.delete_replay(replay);}
void MenuLoop::play_title_music(){owner.music_control().prepare(0,"bgm/th10_02.wav");owner.music_control().play(0,0);}
void MenuLoop::stop_music(){owner.music_control().stop();}
void MenuLoop::update_menu(TitleMenu& t,i32 screen){
    TitleSelection selection{t,owner.selection};switch(screen){
    case 1:t.update_prompt(owner.main);break;case 2:t.update_main(owner.main);break;
    case 4:TitleOptions{t,owner.options}.update();break;case 5:TitleKeys{t,owner.keys}.update();break;
    case 6:selection.difficulty();break;case 7:selection.character();break;case 8:selection.shot();break;
    case 9:
#ifdef TH_ENABLE_THPRAC
    // Advanced practice: the thprac overlay owns the stage-select screen and
    // commits game.stage/practice flags directly, mirroring TitleScene's
    // PracticeStageSelect callback in th08.
    if(owner.state.practice.enabled){
        auto& p=owner.state.practice;auto& game=owner.state.game;
        if(p.accepted){
            p.accepted=false;p.menu=false;p.active=true;p.replay=false;const i32 stage=p.run.stage+1;
            if(stage==7)game.difficulty=4;
            game.flags|=0x10;
            t.set_screen(3,&owner.engine.speed);
            game.stage=stage;game.reserved_040=stage;owner.state.current_stage=owner.data.stages+stage;
            owner.state.pending_screen=7;owner.music_control().fade(6);
        }else p.menu=true;
        break;
    }
#endif
    selection.stage();break;
    case 11:TitleScores{t,owner.score}.update();break;case 12:TitleReplays{t,owner.replays}.update();break;case 14:MusicRoom{t,owner.music}.update();break;
    case 15:if(!owner.results)__builtin_trap();owner.clear.ranking=owner.results;TitleClear{t,owner.clear}.update_rank();break;
    case 16:TitleClear{t,owner.clear}.update_save();break;
    }
}
void MenuLoop::draw_menu(TitleMenu& t,i32 screen){switch(screen){case 9:draw_practice_stages(t,owner.draw);break;case 11:draw_title_scores(t,owner.draw);break;case 12:draw_title_replays(t,owner.draw,owner.data.long_difficulties);break;case 15:draw_title_clear_rank(t,owner.draw);break;case 16:draw_title_clear_save(t,owner.draw,owner.data.long_difficulties);break;}}
MenuResources::MenuResources(Title& o):owner(o){current=&o.value;startup=&o.startup;engine_flags=&o.state.engine_flags;pending_screen=&o.state.pending_screen;menu_state=reinterpret_cast<i32*>(&o.state.quitting);slots=o.engine.manager.files;registry=&o.engine.manager.registry;chain=&o.update_chain;callbacks=&o.engine.callback_environment;update_callback=callback_id::TitleUpdate;draw_callback=callback_id::TitleDraw;loader_callback=callback_id::TitleLoad;title_vtable=1;thread_vtable=2;}
TitleMenu* MenuResources::allocate(){return static_cast<TitleMenu*>(std::malloc(sizeof(TitleMenu)));}
void MenuResources::delete_object(void* object){std::free(object);}
AnmFile* MenuResources::load_animations(i32 slot,const char* name){return owner.engine.manager.load(slot,name,owner.engine.resources);}
void MenuResources::release_animations(AnmFile& file){file.release(owner.engine.resources);}
void MenuResources::delete_replay(Replay* replay){owner.delete_replay(replay);}
void MenuResources::free_file(void* bytes){std::free(bytes);}
void MenuResources::report_error(){owner.error=-1;}
u32 MenuResources::begin_thread(CallbackToken token,void*,u32 flags,u32& id){if(token!=loader_callback||flags)__builtin_trap();owner.loading_task={};owner.loading=true;id=1;return 1;}
u32 MenuResources::wait_thread(u32,u32){owner.loading=false;return 0;}
void MenuResources::close_thread(u32){owner.loading=false;}
void MenuResources::sleep(u32){__builtin_trap();} // Only the resumable loader is scheduled.
Title::Title(GameState& s,AnimationEngine& e,Common& c,Fonts& f,Input& i,Audio& a,Scores& records,ScreenEffects& fx):state(s),engine(e),common(c),fonts(f),input(i),audio(a),scores(records),effects(fx),data(menu_data(s.chinese)),calendar(default_calendar()),update_chain(&e.chain_value),main(*this),selection(*this),keys(*this),options(*this),score(*this),replays(*this),music(*this),draw(*this),clear(*this),loop(*this),resources(*this),writer(records.files,calendar,s.game,s.active_time,s.total_time,s.motion.cheat_movement_used,s.chinese){engine.register_receiver(*this);}
Title::~Title(){if(value){auto* title=value;TitleResources{*title,resources}.shutdown();std::free(title);}while(previews)delete_replay(&previews->document.value);engine.unregister_receiver(*this);}
bool Title::initialize(){if(value)return true;value=TitleResources::create(resources);if(!value)return false;advance_loading();return error==0;}
void Title::advance_loading(){if(loading&&loading_task.advance(resources))loading=false;}
#ifndef TH_NATIVE_PLATFORM
bool Title::invoke(CallbackToken token,void* object,i32& result){if(token==callback_id::TitleUpdate){result=update_title(*static_cast<TitleMenu*>(object),loop);return true;}if(token==callback_id::TitleDraw){result=draw_title(*static_cast<TitleMenu*>(object),loop);return true;}return false;}
#endif
u32 Title::create(AnmFile& file,i32 script){return engine.manager.create(file,script,15,AnimationPlacement::WorldBack,engine,engine);}
void Title::sound(i32 id){audio.manager.queue_effect(id,0,sound_definitions);}
void Title::text(AnmVm& vm,u32 color,const char* pattern,const u32* args,u32 count,TextAlignment alignment){char text[128];if(format_text(text,sizeof(text),pattern,args,count)<0)__builtin_trap();AnmText::draw(vm,color,text,alignment,fonts);}
Replay* Title::preview(const char* name){auto* bytes=std::malloc(sizeof(Preview));if(!bytes)return nullptr;auto* p=new(bytes)Preview(scores.files,state.game.flags,previews);if(p->document.load(name)){p->~Preview();std::free(p);return nullptr;}previews=p;return &p->document.value;}
void Title::delete_replay(Replay* replay){if(!replay)return;for(auto** link=&previews;*link;link=&(*link)->next){auto* p=*link;if(&p->document.value==replay){*link=p->next;p->~Preview();std::free(p);return;}}if(results){results->delete_replay(replay);return;}__builtin_trap();}
AudioGame Title::music_control(){AudioGame game{audio.manager,&scores.data,&state.configuration.display_flags,&engine.speed};
#ifdef TH_ENABLE_THPRAC
    game.practice=&state.practice;
#endif
    return game;}
}

namespace th10::browser {
void Title::bind_callbacks(Callbacks& b){callback_context=this;
 b.bind(callback_id::TitleUpdate,this,[](void* p,void* o,i32){return update_title(*static_cast<TitleMenu*>(o),static_cast<Title*>(p)->loop);});
 b.bind(callback_id::TitleDraw,this,[](void* p,void* o,i32){return draw_title(*static_cast<TitleMenu*>(o),static_cast<Title*>(p)->loop);});
}
}
