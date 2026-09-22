#include "../game/CallbackNames.hpp"
#include "World.hpp"
#include "../game/ResultsResources.hpp"
#include "../game/ResultsDraw.hpp"
#include "../game/AnmCapture.hpp"
#include <new>
namespace th10::browser {
namespace {
struct ResultsServices final:ResultsEnvironment {
    World& w;explicit ResultsServices(World& world):w(world){
        game=&w.state.game;gui=&w.actors.gui;scores=&w.scores.data;replay=&w.state.replay;stages=menu_data(w.state.chinese).stages;current_stage=&w.state.current_stage;
        engine_flags=&w.state.engine_flags;display_flags=&w.state.configuration.display_flags;pending_screen=&w.state.pending_screen;pressed=reinterpret_cast<const u32*>(&w.input.player_profiles[0].input.raw_pressed);repeated=&w.input.player_profiles[0].input.raw_repeat;rate=&w.engine.speed;background_file=&w.common.value->capture;alphabet=menu_data(w.state.chinese).alphabet;active_time=&w.state.active_time;total_time=&w.state.total_time;cheat_movement_used=&w.motion.cheat_movement_used;bind_session();
    }
    void bind_session(){auto* p=w.actors.session;controller_update=p?&p->update_entry:nullptr;controller_elapsed=p?&p->elapsed:nullptr;controller_flags=p?&p->session_flags:nullptr;replay_mode=p?&p->replay_mode:nullptr;}
    void sound(i32 id) override{w.sound(id);}
    void music_command(i32 command,const char* label) override{
#ifdef TH_ENABLE_THPRAC
        // th10 ElBgmTest pause/resume classification: Results::pause issues
        // music command 6 (0x422be1) and Results::resume issues 7 (0x422c51).
        if(command==6){if(practice_bgm_filter(w.state.practice,2,0,false))return;}
        else if(command==7){if(practice_bgm_filter(w.state.practice,3,0,false))return;}
#endif
        w.audio.manager.queue_music(command,0,label);}
    void result_music() override{w.music().prepare(0,"bgm/th10_17.wav");}
    u32 create_animation(AnmFile& file,i32 script) override{return w.engine.manager.create(file,script,15,AnimationPlacement::UiBack,w.engine,w.engine);}
    void capture_background(u32 id) override{auto& manager=w.engine.manager;reinterpret_cast<AnmCapture*>(manager.header)->from_animation(id,{32,16,384,448},manager.registry);}
    void interrupt(u32 id,i32 label) override{w.engine.manager.registry.interrupt(id,static_cast<std::int16_t>(label));}
    u32 child(u32& id,i32 script) override{return w.engine.manager.registry.find_child(id,script);}
    void visible(u32 id,bool visible) override{w.engine.manager.registry.set_visibility(id,visible);}
    void delete_animation(u32 id) override{w.engine.manager.registry.request_delete(id);}
    void select_screen(i32 id) override{w.select_screen(id);}
    Replay* preview(const char* name) override{return w.preview(name);}
    void delete_replay(Replay* replay) override{w.release_replay(replay);}
    void save_replay(const char* file,const char* name) override{w.save_replay(file,name);}
    void timestamp(i32& value) override{value=w.calendar.timestamp();}
};
struct Draw final:ResultsDrawEnvironment {
    World& w;explicit Draw(World& world):w(world){game=&w.state.game;scores=&w.scores.data;replay=&w.state.replay;const auto& data=menu_data(w.state.chinese);alphabet=data.alphabet;characters=data.characters;difficulties=data.difficulties;stage_names=data.stage_names;replay_stage_names=data.replay_stage_names;color=&w.common.value->color;text_mode=reinterpret_cast<u32*>(&w.common.value->shadow);}
    ReplayDate local_date(i32 timestamp) override{return w.calendar.local_date(timestamp);}
    void text(const Vec3& p,const char* format,const u32* args,u32 count) override{w.queue_text(p,format,args,count);}
};
struct Resources final:ResultsResourceEnvironment {
    World& w;explicit Resources(World& world):w(world){current=&w.actors.results;chain=&w.chain;callbacks=&w.engine.callback_environment;update_callback=callback_id::ResultsUpdate;draw_callback=callback_id::ResultsDraw;rate=&w.engine.speed;registry=&w.engine.manager.registry;}
    Results* allocate() override{return static_cast<Results*>(std::malloc(sizeof(Results)));}
    void delete_object(Results* object) override{std::free(object);}
    void delete_replay(Replay* replay) override{w.release_replay(replay);}
};
}
ResultsEnvironment& World::results_services(){if(!results_adapter)results_adapter=new(std::malloc(sizeof(ResultsServices))) ResultsServices(*this);static_cast<ResultsServices*>(results_adapter)->bind_session();return *results_adapter;}
void World::release_results_services(){if(results_adapter){static_cast<ResultsServices*>(results_adapter)->~ResultsServices();std::free(results_adapter);results_adapter=nullptr;}}
bool World::create_results(){Resources env(*this);return ResultsResources::create(env)!=nullptr;}
void World::destroy_results(Results* results){Resources env(*this);ResultsResources{*results,env}.shutdown();std::free(results);}
void World::show_results(bool clear){actors.results->show_result(clear,results_services());}
i32 World::update_results(){return actors.results->update(results_services());}
i32 World::draw_results(){Draw env(*this);return actors.results->draw(env);}
}
