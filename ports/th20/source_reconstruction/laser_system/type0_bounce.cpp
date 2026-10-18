#include "type0.hpp"
#include "../damage_regions/geometry.hpp"
#include "../ecl_vm/math.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include <cmath>
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
bool line(float& slope,float& intercept,float ax,float ay,float bx,float by){const float dx=sub(bx,ax),dy=sub(by,ay);if(std::fabs(dx)>=0.01f){slope=div(dy,dx);intercept=sub(ay,div(n::mul32(dy,ax),dx));}else{slope=0;intercept=ax;}return std::fabs(dx)<0.01f;} //456170
void intersection(float& x,float& y,float ax,float ay,float bx,float by,float cx,float cy,float dx,float dy){ //4545d0
    if(!geometry::segment_segment(ax,ay,bx,by,cx,cy,dx,dy))return;float a,b,c,d;const bool first=line(a,b,ax,ay,bx,by),second=line(c,d,cx,cy,dx,dy);
    if(!first&&!second){x=div(sub(d,b),sub(a,c));y=n::add32(div(n::mul32(sub(d,b),a),sub(a,c)),b);}
    else if(!first||!second){if(!first){x=cx;y=n::add32(n::mul32(a,cx),b);}else{x=ax;y=n::add32(n::mul32(c,ax),d);}}
    else if(std::fabs(sub(ax,cx))<0.001f){x=ax;y=ay;}
}
}
int Type0Laser::bounce(){
    sprite::Vec3 end{};tip_position(end);if(!bullet::outside_viewport(end,0,0))return 0;auto& c=commands[4];bool bounced=false;
    const auto bounce_at=[&](float ax,float ay,float bx,float by,bool vertical){
        if(!(c.field_38&16)){intersection(parameters.position.x,parameters.position.y,ax,ay,bx,by,end.x,end.y,position.x,position.y);parameters.position.z=0;parameters.angle=vertical?m::wrap_angle(sub(-angle,3.1415927410125732f)):-angle;parameters.speed=c.field_10;parameters.radial_offset=0;spawn_type0(*static_cast<Controller*>(context->objects_04[4]),parameters);}bounced=true;
    };
    if((c.field_38&1)&&end.y<0)bounce_at(-256,0,256,0,false);
    if((c.field_38&2)&&end.y>448)bounce_at(-256,448,256,448,false);
    if((c.field_38&4)&&end.x<-192)bounce_at(-192,-192,-192,640,true);
    if((c.field_38&8)&&end.x>192)bounce_at(192,-192,192,640,true);
    if(!bounced)return 0;active_commands&=~std::uint64_t{64};if(parameters.field_4c>=0)program_entry::thread_registry.request_effect(parameters.field_4c,0);return 1;
}
}
