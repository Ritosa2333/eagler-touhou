#pragma once
#include "shot_callbacks.hpp"
namespace th20::source::player_entity {
void advance_fixed_motion(Player&,std::int32_t x,std::int32_t y,float clock_scale); //4ffa10
void update_feedback(Feedback&,ShotCallbackEnvironment&); //4f7210
void update_feedback(Feedback&);
bool shot_corners_outside(const sprite::Vec3(&)[4],std::int32_t viewport_x,std::int32_t viewport_y); //506570
class ShotFrameServices {
public:
    virtual ~ShotFrameServices()=default;
    virtual ShotCallbackEnvironment& callbacks()=0;
    virtual void corners(sprite::Animation&,sprite::Vec3(&)[4])=0;
    virtual std::int32_t viewport_x()=0;
    virtual std::int32_t viewport_y()=0;
};
ShotFrameServices& shot_frame_services();
void update_shot(Shot&,ShotFrameServices&); //504480
void update_shot(Shot&);
}
