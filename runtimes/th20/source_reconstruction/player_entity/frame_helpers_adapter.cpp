#include "frame_helpers.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../sprite_renderer/anm_vm.hpp"
namespace th20::source::player_entity {
namespace {
class GameShotFrame final:public ShotFrameServices {
public:
    ShotCallbackEnvironment& callbacks() override{return shot_callback_environment();}
    void corners(sprite::Animation& animation,sprite::Vec3(&output)[4]) override{sprite::calculate_animation_corners(animation,output);}
    std::int32_t viewport_x() override{return sprite::anm_environment::screen_offset(0,0);}
    std::int32_t viewport_y() override{return sprite::anm_environment::screen_offset(0,1);}
};
}
ShotFrameServices& shot_frame_services(){static GameShotFrame result;return result;}
void update_shot(Shot& shot){update_shot(shot,shot_frame_services());}
void update_feedback(Feedback& feedback){update_feedback(feedback,shot_callback_environment());}
}
