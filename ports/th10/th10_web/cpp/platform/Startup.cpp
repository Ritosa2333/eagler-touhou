#include "../game/CallbackNames.hpp"
#include "Startup.hpp"
#include <cstdlib>
#include <new>
namespace th10::browser {
Startup::Startup(GameState& s,AnimationEngine& e,FileSystem& f,Audio& a):state(s),engine(e),files(f),audio(a),update_chain(&e.chain_value){
    current=&value;common=&common_value;chain=&update_chain;callbacks=&e.callback_environment;
    update_callback=callback_id::StartupUpdate;draw_callback=callback_id::StartupDraw;loader_callback=callback_id::StartupLoad;thread_vtable=1;
    slots=e.manager.files;loading_animations=&loading_file;engine_flags=&s.engine_flags;display_flags=&s.configuration.display_flags;pending_screen=&s.pending_screen;
    music_format=reinterpret_cast<u8**>(&a.manager.formats);music_filename=a.manager.music_filename;
    opening_name="sig.anm";text_name="text.anm";format_name="../../bgm/thbgm.fmt";music_name="thbgm.dat";front_name="front.anm";bullet_name="bullet.anm";
    engine.register_receiver(*this);
}
Startup::~Startup(){if(value){auto* object=value;object->shutdown(*this);std::free(object);}engine.unregister_receiver(*this);}
bool Startup::initialize(){
    if(value)return true;audio.display_flags=state.configuration.display_flags;audio.music_enabled=state.configuration.options[1];audio.effects_enabled=state.configuration.options[2];
    audio.configured_music=state.configuration.music_volume;audio.configured_effects=state.configuration.effects_volume;
    if(audio.begin_loading(0)<0)error=-1;value=StartupScreen::create(*this);return value!=nullptr;
}
void Startup::advance_loading(){if(loading){loading=false;value->load(*this);}}
#ifndef TH_NATIVE_PLATFORM
bool Startup::invoke(CallbackToken token,void* object,i32& result){if(token==update_callback){result=static_cast<StartupScreen*>(object)->update(*this);return true;}if(token==draw_callback){result=static_cast<StartupScreen*>(object)->draw(*this);return true;}return false;}
#endif
StartupScreen* Startup::allocate(){return static_cast<StartupScreen*>(std::malloc(sizeof(StartupScreen)));}
void Startup::delete_object(void* object){std::free(object);}
void Startup::free_bytes(void* bytes){std::free(bytes);}
AnmFile* Startup::load_animations(i32 slot,const char* name){return engine.manager.load(slot,name,engine.resources);}
void Startup::release_animations(AnmFile& file){file.release(engine.resources);}
bool Startup::create_common(){auto* bytes=std::malloc(sizeof(Common));if(!bytes)return false;shared=new(bytes)Common(engine);if(!shared->initialize()){shared->~Common();std::free(shared);shared=nullptr;return false;}common_value=shared->value;return true;}
void Startup::delete_common(CommonResources&){if(shared){shared->~Common();std::free(shared);shared=nullptr;}common_value=nullptr;}
void Startup::create_scores(){auto* bytes=std::malloc(sizeof(Scores));if(bytes)scores=new(bytes)Scores(files,engine.script_random,state.chinese);}
void Startup::save_scores(){if(scores)scores->save();}
void Startup::delete_scores(){if(scores){scores->~Scores();std::free(scores);scores=nullptr;}}
u8* Startup::read_file(const char* name){if(std::strcmp(name,format_name)==0)return audio.load_formats(name)?nullptr:reinterpret_cast<u8*>(audio.manager.formats);return ResourceFiles{files}.load(name,nullptr,false);}
bool Startup::file_exists(const char* name){return ResourceFiles{files}.exists(name);}
void Startup::report(StartupError){error=-1;}
void Startup::initialize_audio(){if(audio.finish_loading()<0)error=-1;}
void Startup::load_music(const char* name){AudioResources{audio.manager,audio.resources}.start_file_music(name);}
u32 Startup::create_opening_animation(AnmFile& file){return engine.manager.create(file,0,15,AnimationPlacement::WorldBack,engine,engine);}
u32 Startup::create_loading_animation(AnmFile& file){return engine.manager.create_at(file,6,{480,392,0},false,AnimationPlacement::WorldBack,engine,engine);}
u32 Startup::begin_thread(CallbackToken token,void*,u32 flags,u32& id){if(token!=loader_callback||flags)__builtin_trap();loading=true;id=1;return 1;}
u32 Startup::wait_thread(u32,u32){loading=false;return 0;}
void Startup::close_thread(u32){loading=false;}
void Startup::sleep(u32){__builtin_trap();}
}

namespace th10::browser {
void Startup::bind_callbacks(Callbacks& b){callback_context=this;
 b.bind(update_callback,this,[](void* p,void* o,i32){return static_cast<StartupScreen*>(o)->update(*static_cast<Startup*>(p));});
 b.bind(draw_callback,this,[](void* p,void* o,i32){return static_cast<StartupScreen*>(o)->draw(*static_cast<Startup*>(p));});
}
}
