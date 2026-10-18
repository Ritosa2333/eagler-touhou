#pragma once
#include "../core_scheduler/scheduler.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/animation.hpp"
#include "../game_session/session.hpp"
namespace th20::source::special_state {
// 511930: byte interpolation has alignment padding which the constructor keeps.
struct ByteInterpolation {
    std::uint8_t start,end,tangent_start,tangent_end,current,padding[3];
    recovered::Timer timer;
    std::int32_t duration,mode;
};
using FloatInterpolation=sprite::Interpolation<float>;
struct Entry {
    scheduler::Link link; // intrusive value points to this entry, not a scheduler Node
    std::uint32_t enemy_handle,animation_handle;
    recovered::Timer age;
    std::int32_t color,level;
};
struct Controller {
    scheduler::List entries;
    recovered::Timer age,charge_age;
    FloatInterpolation text_expansion;
    ByteInterpolation text_alpha;
    FloatInterpolation meter_remaining;
    std::uint8_t active,padding_b1[3];
};
static_assert(sizeof(ByteInterpolation)==0x20&&offsetof(ByteInterpolation,timer)==8);
static_assert(sizeof(Entry)==0x34&&offsetof(Entry,color)==0x2c);
static_assert(sizeof(Controller)==0xb4&&offsetof(Controller,text_alpha)==0x64&&offsetof(Controller,active)==0xb0);
void construct(ByteInterpolation&) noexcept; //511930
void construct(Entry&) noexcept; //5119e0
void construct(Controller&) noexcept; //511980
void start(ByteInterpolation&,int duration,int mode,std::uint8_t from,std::uint8_t to); //5141e0
std::uint8_t evaluate(ByteInterpolation&); //511af0, does not tick timer
std::uint8_t sample(ByteInterpolation&,const float* rate); //513710
float evaluate(FloatInterpolation&); //511cf0, does not tick timer
void start(FloatInterpolation&,int duration,int mode,float from,float to); //439550
float sample(FloatInterpolation&,const float* rate); //42a110
unsigned color(Controller&,game_session::Player&); //513f70, mutating Player clamps
inline bool is_active(const Controller& value) noexcept{return value.active!=0;} //485a40
extern Controller* controller; // unique 5c6118, initialized by514240
class Environment;
Environment& environment();
Controller* create(); //514240->5117e0
void destroy(Controller&,Environment&); //511a30
void release(); //513cc0->511770
void retire(Entry&,Environment&); //512f30
int update_entry(Entry&,Environment&); //512980
void update(Controller&,Environment&); //5120d0
void update(Controller&);
void draw(Controller&,Environment&); //512aa0
void draw(Controller&);
Entry* attach(Controller&,unsigned enemy_handle,int color,Environment&); //512f60->512fb0
int collect(Entry&,void* enemy,Environment&); //511f00
}
