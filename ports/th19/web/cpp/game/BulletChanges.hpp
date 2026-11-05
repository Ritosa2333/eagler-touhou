#pragma once
#include "Types.hpp"
#include "AnmVm.hpp"
#include <array>
#include <vector>
#include <cmath>
namespace th19 {
struct BulletChange {
    std::array<float,4> floats{};
    std::array<i32,4> integers{};
    u32 flags=0; i32 concurrent=0;
};
inline bool supportedBulletChange(const BulletChange&c){
    // Angle sentinel handling in 0x4f3750 is separate from literal radians.
    const bool fixedAcceleration=(c.flags==4||(c.flags==0x200000&&c.integers[0]>0))&&std::isfinite(c.floats[0])&&
        std::isfinite(c.floats[1])&&(c.floats[1]<999990.f||
        (c.floats[1]<1999990.f&&std::isfinite(c.floats[2])));
    const bool bounce=c.flags==0x40&&std::isfinite(c.floats[0])&&
        !(u32(c.integers[1])&~0x3fu)&&(!(c.integers[1]&0x20)||
        (std::isfinite(c.floats[1])&&std::isfinite(c.floats[2])&&c.floats[1]>0&&c.floats[2]>0));
    const bool redirect=c.flags==0x10&&std::isfinite(c.floats[0])&&c.floats[0]<999990.f&&
        std::isfinite(c.floats[1])&&(c.integers[2]==0||c.integers[2]==1||c.integers[2]==4||c.integers[2]==5||c.integers[2]==6);
    const bool morph=c.flags==0x200&&c.integers[0]>=0&&c.integers[0]<48&&
        c.integers[1]>=0&&(c.integers[1]&0x7fff)<16;
    const bool homing=c.flags==0x40000002u&&std::isfinite(c.floats[0])&&
        std::isfinite(c.floats[1])&&std::isfinite(c.floats[2]);
    return (c.flags==0x80000&&c.integers[0]>0&&std::isfinite(c.floats[0])&&std::isfinite(c.floats[1])) || homing || c.flags==0x400 || c.flags==1 || morph || c.flags==0x800 || c.flags==0x100000 || (c.flags==0x100&&(c.integers[1]==0||c.integers[1]==1)) || redirect || c.flags==0 || c.flags==2 || c.flags==0x80 || c.flags==8 || c.flags==0x40000001u || fixedAcceleration || bounce;
}
// Verified subset of 0x4f1460 / 0x4f5680. Independent native state slots:
// vector acceleration (4), angular/scalar acceleration (8), waiting (high bit 1).
struct BulletChanges {
    std::array<BulletChange,24> records{};
    std::vector<BulletChange> immediate_events;
    struct Action {BulletChange record;float elapsed=0;bool active=false;};
    std::array<Action,10> actions{};
    float redirect_speed=0,redirect_angle=0;int redirects=0;
    int cursor=0;bool active=false;
    int animation_interrupt=-1,native_timer=-1;
    float speed=0,angle=0;
    int begin(float initialSpeed,float initialAngle,float x=0,float y=224,float player_x=0,float player_y=400){
        speed=initialSpeed;angle=initialAngle;cursor=0;actions={};immediate_events.clear();active=false;return advance(x,y,player_x,player_y);
    }
    bool preserves_bounds()const{return actions[6].active;}
    void refresh(){active=false;for(const auto&a:actions)active|=a.active;}
    int advance(float x=0,float y=224,float player_x=0,float player_y=400){
        int sounds=0;
        while(cursor<24){const auto&c=records[cursor];if(c.flags==0)break;
            refresh();
            // 0x4f1460 masks out 0x100 when deciding whether the next record waits.
            bool blocking=false;for(int i=0;i<int(actions.size());++i)if(i!=6)blocking|=actions[i].active;
            if(c.concurrent==0&&blocking)break;
            if(c.flags==0x400||c.flags==0x200||c.flags==0x800||c.flags==0x100000){immediate_events.push_back(c);++cursor;continue;}
            if(c.flags==2){animation_interrupt=static_cast<short>(c.integers[0])+7;++cursor;continue;}
            if(c.flags==0x80){native_timer=c.integers[0];++cursor;continue;}
            const int slot=c.flags==0x80000?9:c.flags==0x40000002u?8:c.flags==1?7:c.flags==4?0:c.flags==8?1:c.flags==0x40?3:c.flags==0x10?2:c.flags==0x200000?5:c.flags==0x100?6:4;
            // 0x4f1460 tests the low active-mask against the raw opcode.
            // High-bank wait can be replaced when concurrent is nonzero.
            if(slot!=4&&slot!=8&&actions[slot].active)break;
            const int index=cursor++;
            if(slot==2){redirect_speed=c.floats[1]>-999990.f?c.floats[1]:speed;
                redirect_angle=c.floats[0]>-999990.f?c.floats[0]:angle;
                // 4f1460 modes5/6 sample a signed random turn ONCE at activation.
                // 4f0ad8 maps5 to relative turn,6 to live aimed turn.
                if(c.integers[2]==5||c.integers[2]==6)redirect_angle=(AnmVm::rand_unit()*2.f-1.f)*c.floats[0];
                redirects=0;}
            if(slot!=4||c.integers[0]>0){
                actions[slot]={c,0,true};
                // 0x4f2d62: target speed becomes vector acceleration at activation.
                if(slot==5)actions[slot].record.floats[0]=(c.floats[0]-speed)/c.integers[0];
                // 0x4f3750: <= -999990 preserves the current bullet angle.
                if(slot==0||slot==5){
                    float&direction=actions[slot].record.floats[1];
                    if(c.floats[1]<=-999990.f)direction=angle;
                    else if(c.floats[1]>=999990.f&&c.floats[1]<1999990.f){
                        // 0x4f3750: aimed marker, sampled when this action starts.
                        constexpr float pi=3.14159265358979323846f;
                        const float offset=c.floats[2]<=-999990.f?pi:c.floats[2];
                        const float dx=player_x-x,dy=player_y-y;
                        direction=(dx==0&&dy==0?pi*.5f:std::atan2(dy,dx))+offset;
                    }
                }
                if((slot<2||slot==5)&&index>0)++sounds;
            }
        }
        refresh();return sounds;
    }
    int tick(float&vx,float&vy){float x=0,y=224;return tick(vx,vy,x,y);}
    int tick(float&vx,float&vy,float&x,float&y,float player_x=0,float player_y=400,float time_scale=1,float sprite_width=0,float sprite_height=0){
        int sounds=0;
        // Native repeats the whole update pass after any action completes,
        // including still-active parallel actions. Position integrates outside.
        for(int guard=0;guard<25;++guard){sounds+=advance(x,y,player_x,player_y);bool completed=false;
            for(int slot:{7,0,5,1,2,3,8,9,6,4}){auto&a=actions[slot];if(!a.active)continue;
                const auto&c=a.record;
                if(slot==9){ // 4ef330: additive position vector, independent of heading/velocity.
                    x+=std::cos(c.floats[0])*c.floats[1]*time_scale;
                    y+=std::sin(c.floats[0])*c.floats[1]*time_scale;
                    // Native calls402650(0), resetting this timer each pass, not incrementing it.
                    // Only the verified positive-duration branch is accepted.
                    a.elapsed=0;continue;
                }
                if(slot==8){ // 0x4ef6d0: interpolate velocity toward a live aimed vector.
                    if(a.elapsed>=c.integers[0]){a.active=false;completed=true;continue;}
                    constexpr float pi=3.14159265358979323846f;
                    const float dx=player_x-x,dy=player_y-y;
                    float target=(dx==0&&dy==0?pi*.5f:std::atan2(dy,dx))+c.floats[1];
                    for(int i=0;target>pi&&i<33;++i)target-=2*pi;
                    for(int i=0;target< -pi&&i<33;++i)target+=2*pi;
                    vx+=(std::cos(target)*c.floats[0]-vx)*c.floats[2];
                    vy+=(std::sin(target)*c.floats[0]-vy)*c.floats[2];
                    speed=std::hypot(vx,vy);angle=std::atan2(vy,vx);
                    a.elapsed+=time_scale;continue;
                }
                if(slot==7){ // 0x4f5680: temporary +5 boost, timer 0..16 inclusive.
                    // Descriptor speed remains unchanged for following changes.
                    if(int(a.elapsed)>16){a.active=false;completed=true;continue;}
                    const float velocity=speed+5.f-float(a.elapsed)*5.f/16.f;
                    vx=std::cos(angle)*velocity;vy=std::sin(angle)*velocity;
                    a.elapsed+=time_scale;continue;
                }
                if(slot==6){ // 4eeda0: countdown, then native forward-corner test for subtype1.
                    a.elapsed+=time_scale;
                    bool expires=a.elapsed>=c.integers[0];
                    if(c.integers[1] && (x+sprite_width*.5f<=-148 || x-sprite_width*.5f>=148 ||
                       y+sprite_height*.5f<=0 || y-sprite_height*.5f>=448)){
                        bool left=false,right=false;const float hx=std::cos(angle),hy=std::sin(angle);
                        for(float cx:{-148-sprite_width*.5f,148+sprite_width*.5f})
                            for(float cy:{-sprite_height*.5f,448+sprite_height*.5f}){
                                float dx=cx-x,dy=cy-y,length=std::hypot(dx,dy);
                                if(length>0){dx/=length;dy/=length;}
                                const float cross=hx*dy-hy*dx,dot=hx*dx+hy*dy;
                                if(dot>=0){left|=cross<=0;right|=cross>=0;}
                            }
                        expires|=!left||!right;
                    }
                    if(expires){a.active=false;completed=true;}continue;
                }
                if(slot==3){
                    // 0x4efb70: point bounds, top/left strict, bottom/right inclusive.
                    // Default 296 x 448, centred at (0,224); flag 0x20 supplies dimensions.
                    const int flags=c.integers[1];const float w=(flags&0x20)?c.floats[1]:296.f,h=(flags&0x20)?c.floats[2]:448.f;
                    const float left=-w*.5f,right=w*.5f,top=224-h*.5f,bottom=224+h*.5f;
                    if(x>left&&x<right&&y>top&&y<bottom)continue;
                    bool hit=false;constexpr float pi=3.14159265358979323846f;
                    auto reflect=[&](bool condition,bool vertical,float&pos,float twiceBoundary){
                        if(!condition)return;hit=true;
                        if(!(flags&0x10)){angle=vertical?-angle:-angle-pi;pos=twiceBoundary-pos;
                            for(int i=0;angle>pi&&i<33;++i)angle-=2*pi;
                            for(int i=0;angle< -pi&&i<33;++i)angle+=2*pi;}
                    };
                    reflect((flags&1)&&y<top,true,y,448-h);
                    reflect((flags&2)&&y>=bottom,true,y,448+h);
                    reflect((flags&8)&&x>=right,false,x,w);
                    reflect((flags&4)&&x<left,false,x,-w);
                    if(c.floats[0]>-990.f)speed=c.floats[0];
                    vx=std::cos(angle)*speed;vy=std::sin(angle)*speed;
                    if(hit){++a.elapsed;++sounds;}
                if(a.elapsed>=c.integers[0]){a.active=false;completed=true;}
                    continue;
                }
                if(slot==2){
                    float velocity=speed;
                    if(a.elapsed<c.integers[0])velocity=speed*(1.f-float(a.elapsed)/c.integers[0]);
                    else{++redirects;++sounds;
                        if(c.integers[2]==0||c.integers[2]==5)angle+=redirect_angle;
                        else if(c.integers[2]==1||c.integers[2]==6)angle=std::atan2(player_y-y,player_x-x)+redirect_angle;
                        else angle=redirect_angle;
                        angle=std::remainder(angle,6.28318530717958647692f);
                        speed=velocity=redirect_speed;a.elapsed=0;
                        if(redirects>=c.integers[1]){a.active=false;completed=true;}}
                    vx=std::cos(angle)*velocity;vy=std::sin(angle)*velocity;
                    if(a.active)a.elapsed+=time_scale;continue;
                }
                if(a.elapsed>=c.integers[0]){a.active=false;completed=true;continue;}
                if(slot==0||slot==5){
                    speed+=c.floats[0]*time_scale;
                    vx+=std::cos(c.floats[1])*c.floats[0]*time_scale;
                    vy+=std::sin(c.floats[1])*c.floats[0]*time_scale;
                    if(std::abs(vx)>0.0001f||std::abs(vy)>0.0001f){
                        angle=std::atan2(vy,vx);speed=std::sqrt(vx*vx+vy*vy);
                    }
                }else if(slot==1){
                    angle+=c.floats[1]*time_scale;constexpr float pi=3.14159265358979323846f;
                    for(int i=0;angle>pi&&i<33;++i)angle-=2*pi;
                    for(int i=0;angle< -pi&&i<33;++i)angle+=2*pi;
                    speed+=c.floats[0]*time_scale;vx=std::cos(angle)*speed;vy=std::sin(angle)*speed;
                }
                a.elapsed+=time_scale;
            }
            refresh();if(!completed)break;
        }
        return sounds;
    }
};
}
