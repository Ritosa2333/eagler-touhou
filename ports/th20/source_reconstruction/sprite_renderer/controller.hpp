#pragma once
#include "sprite.hpp"
#include "../core_scheduler/scheduler.hpp"
namespace th20::source::sprite {
struct UncoloredVertex24 {float x,y,z,rhw,u,v;};
static_assert(sizeof(UncoloredVertex24)==24);
extern UncoloredVertex24 uncolored_quad[4]; // BSS 0x5c0030; rhw/uv initialized by447e00
// Constructor preserves padding/untouched scalar bytes just as 447e00 does;
// factory 418830 zeroes the entire allocation before invoking it.
void construct_controller(Controller&,scheduler::State&,scheduler::Environment&,Device&);
void destroy_controller_contents(Controller&); //449260, caller owns storage release
Controller* create_controller(scheduler::State&,scheduler::Environment&,Device&); //418830
void destroy_controller(Controller*); //418730->418890->419350->449260
namespace controller_environment {
bool suppress_primary_update(); //nullable GameController5ba828 with424030 &&424060
}
}
