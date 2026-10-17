#include "dialogue.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../runtime_state/state.hpp"
#include <new>
namespace th20::source::hud {
Dialogue::~Dialogue(){
    auto& c=environment::sprites();
    for(unsigned i=0;i<4;++i){sprite::request_animation_deletion(c,portraits[i]);sprite::request_animation_deletion(c,portrait_overlays[i]);}
    for(unsigned i=0;i<7;++i)sprite::request_animation_deletion(c,handles[i]);
}
void destroy_dialogue(Dialogue* value){if(!value)return;value->~Dialogue();std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(value);}
bool update_dialogue(Dialogue& value){const auto result=run_dialogue(value);if(result==0)recovered::timer_tick(value.timers[0],state::timer_rate);return result!=0;}
}
