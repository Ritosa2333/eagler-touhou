#pragma once
#include "DialogueAnmHost.hpp"
#include "StageDefinition.hpp"
#include <memory>
namespace th15 {
struct MessageSceneState {u32 game_flags=0,spell_flags=0;i32 transition_counter=0;bool restart_music=false;std::string current_music_wave;};
struct MessageSceneServices {
 virtual ~MessageSceneServices()=default;
 virtual bool queue_music_control(i32 kind,i32 value)=0;
 virtual bool unlock_music(i32 index)=0;
 virtual bool complete_stage()=0;
 virtual bool begin_game_over()=0;
};
class MessageController {
 const MessageProgram& program;const StageDefinition& definition;DialogueResources resources;
 AnmManager& animations;DialogueAnmHost& host;MessageSceneState& scene;MessageSceneServices& services;
 bool fail(const char*);
public:
 std::unique_ptr<Dialogue> dialogue; i32 script_index=-1;std::string error;
 MessageController(const MessageProgram& p,const StageDefinition& d,DialogueResources r,AnmManager& a,DialogueAnmHost& h,MessageSceneState& s,MessageSceneServices& w):program(p),definition(d),resources(r),animations(a),host(h),scene(s),services(w){}
 ~MessageController();
 bool request(i32 id);bool update(const DialogueInput&);
 bool finished()const noexcept{return !dialogue||dialogue->state.complete!=0;}
 bool active()const noexcept{return dialogue!=nullptr;}
 bool release();
};
}
