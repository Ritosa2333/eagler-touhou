#include "StagePauseActions.hpp"
namespace th15 {
bool StagePauseActions::resume_dialogue(){
 if(!scene.messages.dialogue)return true;
 for(const auto handle:scene.messages.dialogue->state.handles)if(!animations.pause(handle,false))return false;
 return true;
}
}
