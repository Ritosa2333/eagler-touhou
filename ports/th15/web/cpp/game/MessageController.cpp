#include "MessageController.hpp"
namespace th15 {
bool MessageController::fail(const char* message){if(error.empty())error=message;return false;}
MessageController::~MessageController(){release();}
bool MessageController::release(){bool ok=true;if(dialogue){ok=host.release(dialogue->state);dialogue.reset();}script_index=-1;return ok||fail("Dialogue release failed");}
bool MessageController::request(i32 id){
 if(!error.empty())return false;
 if(id==-2)return (scene.spell_flags&0x80?services.begin_game_over():services.complete_stage())||fail("Message scene transition failed");
 if(id==-1||id==-3){const bool boss=id==-1;const u32 index=boss?1:0;
  if((scene.game_flags&0x30)==0x20&&scene.transition_counter==0&&scene.current_music_wave==std::string(definition.music[index])+".wav")return true;
  if(scene.restart_music&&!services.queue_music_control(4,0))return fail("Message music reset failed");
  if(!services.queue_music_control(2,boss?1:0)||!services.unlock_music(definition.music_unlock[index]))return fail("Message music request failed");
  if(!animations.create(resources.logo,boss?2:1,-1,0))return fail("Message music logo failed");return true;
 }
 if(id<0||u32(id)>=program.scripts.size())return fail("Dialogue script index outside message program");
 if(!release())return false;
 dialogue=std::make_unique<Dialogue>(program.scripts[u32(id)],host,resources);script_index=id;
 if(!dialogue->initialize())return fail(dialogue->error.c_str());return true;
}
bool MessageController::update(const DialogueInput& input){if(!error.empty())return false;if(!dialogue)return true;const int result=dialogue->update(input);if(result==-2)return fail(dialogue->error.c_str());if(result==-1)return release();dialogue->state.age.tick(&input.rate);return true;}
}
