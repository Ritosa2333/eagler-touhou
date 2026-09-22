#include "../game/CallbackNames.hpp"
#include "World.hpp"
#include "../game/ReplayResources.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../game/PracticeConfig.hpp"
#endif
#include <new>
namespace th10::browser {
namespace {
struct Gameplay final:ReplayEnvironment {
    World& w;explicit Gameplay(World& world):w(world){game=&w.state.game;input=&w.input.player_profiles[0].input;random=&w.engine.script_random;rate=&w.engine.speed;display_flags=&w.state.configuration.display_flags;recording_mode=reinterpret_cast<const u32*>(&w.new_game);controller_flags=w.actors.session?&w.actors.session->session_flags:nullptr;measured_fps=&w.measured_fps;player=&w.actors.player;}
    void* allocate(u32 bytes) override{return w.replay_memory.allocate(bytes);}
    void release(void* bytes) override{if(!bytes)return;if(w.replay_memory.owns(bytes))w.replay_memory.release(bytes);else w.replay_files.memory.release(bytes);}
    void configure_options(Player&) override{w.configure_player();}
    void activate_player(Player&) override{w.activate_player();}
    void draw_rate(const Vec3& position,u32 color,u8 fps) override{w.common.value->color=color;const u32 argument=fps;w.queue_text(position,"%3d",&argument,1);w.common.value->color=0xffffffff;}
    void timestamp(i32& destination) override{destination=w.calendar.timestamp();}
};
struct Resources final:ReplayResourceEnvironment {
    World& w;Gameplay services;explicit Resources(World& world):w(world),services(world){gameplay=&services;current=&w.state.replay;configuration=w.actors.session?w.actors.session->configuration:nullptr;chain=&w.chain;callbacks=&w.engine.callback_environment;input_callback=callback_id::ReplayUpdate;end_frame_callback=callback_id::ReplayFrame;draw_callback=callback_id::ReplayDraw;}
    i32 load(Replay& replay,const char* name) override{
        w.replay_files.flags=w.state.game.flags;const auto result=load_replay(replay,name,w.replay_files);if(result)return result;
        char path[264];const bool demo=(w.state.game.flags&0x20)!=0;const auto length=std::strlen(name);if(length>255)return -1;const auto prefix=demo?0:7;if(prefix)std::memcpy(path,"replay/",prefix);std::memcpy(path+prefix,name,length+1);const auto handle=w.scores.files.host.open(path,false);
        if(handle!=0xffffffff){const auto size=w.scores.files.host.size(handle);if(size>16*1024*1024){w.scores.files.host.close(handle);return -1;}std::vector<u8> bytes(size);const bool complete=w.scores.files.host.read(handle,bytes.data(),size)==size;w.scores.files.host.close(handle);if(!complete||!w.motion.load(bytes.data(),size,10))return -1;
#ifdef TH_ENABLE_THPRAC
        // THGuiRep::State(2)/(3) restore: the raw file carries the 'USER'/'PRAC'
        // block after the packed replay (practice_replay_read, upstream
        // ReplayLoadParam). Playback is authoritative for the live run, so a
        // valid block is copied into both the menu candidate and the run.
        if(w.state.practice.replay){
            w.state.practice.replay_candidate.reset();w.state.practice.replay_candidate_valid=false;w.state.practice.run.reset();w.state.practice.active=false;
            PracticeConfig config;
            if(practice_replay_read(bytes.data(),u32(size),config)){
                w.state.practice.replay_candidate=config;w.state.practice.replay_candidate_valid=true;
                w.state.practice.run=config;w.state.practice.active=true;
            }
        }
#endif
        }else w.motion.clear();return 0;
    }
};
}
bool World::create_replay(i32 mode,const char* name){motion.clear();Resources env(*this);return ReplayResources::create(mode,name,env)!=nullptr;}
void World::destroy_replay(Replay* replay){if(!replay)return;Resources env(*this);ReplayResources{*replay,env}.shutdown();env.services.release(replay);replay_files.close();replay_files.memory.clear();}
void World::prepare_replay(){Gameplay env(*this);state.replay->prepare_stage(env);}
void World::activate_replay(){Gameplay env(*this);state.replay->activate_stage(env);motion.begin(state.game.stage,false,state.replay->mode!=0,state.replay->mode==0);}
i32 World::update_replay(){Gameplay env(*this);return state.replay->update_input(env);}
i32 World::replay_frame_action(){Gameplay env(*this);return state.replay->frame_action(env,true);}
i32 World::draw_replay(){
    Gameplay env(*this);const i32 result=state.replay->draw(env,true);
    if(motion.playing&&common.value){
        touhou::input::MotionTrack::TouchPoint points[10];const int count=motion.replay_points(points,10);
        auto& text=*common.value;const u32 color=text.color;const Vec2 scale=text.scale;const i32 shadow=text.shadow;
        text.color=0xffffffff;text.scale={.7f,.7f};text.shadow=0;
        for(int i=0;i<count;++i)text.queue("+",{points[i].x*640.f-5.f,points[i].y*480.f-7.f,0},false);
        text.color=color;text.scale=scale;text.shadow=shadow;
    }
    return result;
}
void World::finish_replay(i32 clear){Gameplay env(*this);state.replay->finish_recording(clear,env);}
Replay* World::preview(const char* name){auto* entry=new(std::malloc(sizeof(Preview))) Preview(scores.files,state.game.flags,previews);if(entry->document.load(name)){entry->~Preview();std::free(entry);return nullptr;}entry->document.value.mode=2;previews=entry;return &entry->document.value;}
void World::release_replay(Replay* replay){if(!replay)return;for(auto** next=&previews;*next;next=&(*next)->next){auto* entry=*next;if(&entry->document.value==replay){*next=entry->next;entry->~Preview();std::free(entry);return;}}if(replay==state.replay){destroy_replay(replay);return;}__builtin_trap();}
void World::save_replay(const char* file,const char* name){
#ifdef TH_ENABLE_THPRAC
    const auto motion_tail=motion.playing?std::vector<u8>{}:motion.trailer(10);
    std::vector<u8> tail;
    // Advanced-practice runs append their 'USER'/'PRAC' block immediately after
    // the vanilla replay, ahead of the motion trailer (upstream THSaveReplay +
    // ReplaySaveParam). Assisted/cheated runs are deliberately not savable.
    const bool practice_save=state.practice.active&&!state.practice.replay&&!state.practice.assisted&&state.practice.run.mode==1;
    if(practice_save)tail=practice_replay_block(state.practice.run);
    tail.insert(tail.end(),motion_tail.begin(),motion_tail.end());
    // th10_rep_power_fix runs inside save_replay while the payload is plain.
    replay_writer.practice_mode=practice_save;
    replay_writer.practice_power=state.practice.run.power;
    if((motion.used()&&!motion.playing&&motion_tail.empty())||replay_writer.save(*state.replay,file,name,tail))fail();
#else
    const auto tail=motion.playing?std::vector<u8>{}:motion.trailer(10);if((motion.used()&&!motion.playing&&tail.empty())||replay_writer.save(*state.replay,file,name,tail))fail();
#endif
}
}
