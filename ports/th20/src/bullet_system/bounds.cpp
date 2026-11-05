#include "movement.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include <cmath>
namespace th20::source::bullet {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
const sprite::Vec2& bounds(Bullet& b){return static_cast<Controller*>(b.context->primary_owner)->vector_4c;}
bool outside(const sprite::Vec3& p,float width,float height){return p.x<=div(-width,2)||p.x>=div(width,2)||p.y<=sub(224,div(height,2))||p.y>=n::add32(div(height,2),224);}
int bounce_vertical(Bullet& b,bool bottom){auto& c=b.commands[4];const auto height=bounds(b).y>0?bounds(b).y:c.vector_18.y;
    const float edge=bottom?n::add32(224,div(height,2)):sub(224,div(height,2));
    if(bottom?!(b.position.y>=edge):!(edge>b.position.y))return 0;
    if(!(c.field_3c&16u)){b.angle=m::wrap_angle(-b.angle);b.position.y=sub(bottom?n::add32(448,height):sub(448,height),b.position.y);}return 1;
}
int bounce_horizontal(Bullet& b,bool right){auto& c=b.commands[4];const auto width=bounds(b).x>0?bounds(b).x:c.vector_18.x;
    if(right?!(b.position.x>=div(width,2)):!(div(-width,2)>b.position.x))return 0;
    if(!(c.field_3c&16u)){b.angle=m::wrap_angle(sub(-b.angle,3.1415927410125732f));b.angle=m::wrap_angle(m::wrap_angle(n::add32(b.angle,0)));b.position.x=sub(right?width:-width,b.position.x);}return 1;
}
sprite::Vec2 normalized(sprite::Vec2 v){const auto length=m::square_root(n::add32(n::mul32(v.x,v.x),n::mul32(v.y,v.y)));if(std::fabs(length)>=0.009999999776482582f){v.x=div(v.x,length);v.y=div(v.y,length);}return v;}
}
namespace unrecovered {
int update_bounce_00482a60(Bullet& b){auto& c=b.commands[4];
    if(!(bounds(b).x<=0&&outside(b.position,c.vector_18.x,c.vector_18.y))){if(bounds(b).x<=0||!outside(b.position,bounds(b).x,bounds(b).y))return 0;}
    bool bounced=false;if(c.field_3c&1u)bounced=bounce_vertical(b,false)!=0||bounced;
    if(c.field_3c&2u)bounced=bounce_vertical(b,true)!=0||bounced;
    if(c.field_3c&8u)bounced=bounce_horizontal(b,true)!=0||bounced;
    if(c.field_3c&4u)bounced=bounce_horizontal(b,false)!=0||bounced;
    if(bounced){if(c.field_10>-990)b.field_20=c.field_10;m::polar(b.velocity.x,b.velocity.y,b.angle,b.field_20);++c.field_30;if(n::signed_bits(b.field_40)>=0)program_entry::thread_registry.request_effect(n::signed_bits(b.field_40),0);}
    if(n::signed_bits(c.field_30)<n::signed_bits(c.field_34))return 0;b.field_90&=~std::uint64_t{64};return 1;
}
int update_offscreen_delay_00481200(Bullet& b){auto& c=b.commands[11];n::timer_add(c.timer,-1,state::timer_rate);
    if(c.field_30){const auto& s=sprite::current_sprite(*program_entry::sprite_controller,*b.animation);
        if(outside_viewport(b.position,div(s.extent_4c,2),div(s.extent_48,2))){
            sprite::Vec2 direction{};m::polar(direction.x,direction.y,b.angle,1);float crosses[4],dots[4];
            for(unsigned i=0;i<4;++i){
                const auto x=sub(div((i&1)?n::add32(384,s.extent_4c):sub(-384,s.extent_4c),2),b.position.x);
                const auto y=sub(n::add32(224,div((i&2)?n::add32(448,s.extent_48):sub(-448,s.extent_48),2)),b.position.y);
                const auto corner=normalized({x,y});crosses[i]=sub(n::mul32(direction.x,corner.y),n::mul32(direction.y,corner.x));dots[i]=n::add32(n::mul32(direction.x,corner.x),n::mul32(direction.y,corner.y));
            }
            float negative=-999,positive=-999;
            for(unsigned i=0;i<4;++i)if(crosses[i]<=0&&negative<dots[i]&&dots[i]>=0)negative=dots[i];
            for(unsigned i=0;i<4;++i)if(crosses[i]>=0&&positive<dots[i]&&dots[i]>=0)positive=dots[i];
            if(negative<-998||positive<-998){b.field_90&=~std::uint64_t{0x100};return 1;}
        }
    }
    if(c.timer.current<=0){b.field_90&=~std::uint64_t{0x100};return 1;}return 0;
}
}
}
