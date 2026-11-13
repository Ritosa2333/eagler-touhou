#include "LaserState.hpp"
#include "BulletTh14.hpp"
#include "AnmRenderer.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 14
namespace {
float scalar(const u8* p){float v;std::memcpy(&v,p,4);return v;}
i32 integer(const u8* p){i32 v;std::memcpy(&v,p,4);return v;}
i32 sprite_callback(AnmVm& vm,i32 frame){
    const auto& l=*reinterpret_cast<Th14LaserInfinite*>(reinterpret_cast<u8*>(&vm)-offsetof(Th14LaserInfinite,body));
    if(u32(l.base.sprite)>=43)return -1;const auto& a=th14_bullet_appearances[l.base.sprite];
    if(a.sprites[0]<0)return frame;const i32 index=l.base.color*4+frame;return u32(index)<64?a.sprites[index]:-1;
}
bool visible(Vec3 p,float half){return p.x+half>-192.f&&p.x-half<192.f&&p.y+half>0.f&&p.y-half<448.f;}
}
void laser_th14_infinite_initialize_state(Th14LaserInfinite& l,const u8* request,LaserWorld& world)noexcept{
    std::memcpy(l.request,request,0x374);auto& b=l.base;b.state=3;b.type=1;
    b.sprite=i16(integer(request+0x50));b.color=i16(integer(request+0x54));
    std::memcpy(&b.position,request,12);
    // 43df6f skips the polar branch for an ordered zero offset.
    // The base object's separate +0x7c stays zero in this initializer.
    const float offset=scalar(request+0x4c);
    if(offset!=0.f){const auto v=polar(scalar(request+0x18),offset);b.position.x+=v.x;b.position.y+=v.y;}
    b.angle=scalar(request+0x18);b.length=scalar(request+0x24);b.width=2.f;b.speed=scalar(request+0x2c);b.id=integer(request+0x48);
    b.lifetime.set(0,&world.rate);b.graze_timer.set(0,&world.rate);b.transform_index=0;b.active_transforms=0;
}
bool laser_th14_infinite_initialize(Th14LaserInfinite& l,const u8* request,AnmResource& resource,AnmEnvironment& animations,LaserWorld& world,u16 file){
    laser_th14_infinite_initialize_state(l,request,world);auto& b=l.base;
    if(u32(b.sprite)>=43||u32(b.color)>=16)return false;
    auto& body=l.body;
    if(!body.bind_script(resource,th14_bullet_appearances[b.sprite].script,file,&animations.rate))return false;
    body.sprite_callback=sprite_callback;body.reserved_430=u32(uintptr_t(&l));
    body.interrupt(2);if(body.update(animations)<0)return false;
    body.flags=(body.flags&~(0x70u|0x3c0000u))|0x10u|0x200000u;body.th13_native_flags&=~0x100u;body.set_drawing_mode(1);body.flags2=(body.flags2&~0x40000u)|0x20000u;
    auto& origin=l.origin;
    if(!origin.copy_script(resource,b.color+56,file,&animations.rate)||origin.update(animations)<0)return false;
    origin.interrupt(2);if(origin.update(animations)<0)return false;
    origin.flags=(origin.flags&~(0x70u|0x3c0000u))|0x10u;origin.th13_native_flags&=~0x100u;origin.set_drawing_mode(1);origin.flags2=(origin.flags2&~0x40000u)|0x20000u;
    const i32 sound=integer(request+0x40);if(sound>=0)world.sound(sound,0,true);return true;
}
i32 laser_th14_infinite_step(Th14LaserInfinite& l,LaserWorld& world)noexcept{
    auto& b=l.base;const auto* r=l.request;
    // 43fd20 has exactly three handled opcodes; all others advance.
    for(;b.transform_index<18;++b.transform_index){
        if(b.transform_index<0)return -2;const u8* t=r+0x5c+b.transform_index*0x2c;
        const u32 op=u32(integer(t+0x20));if(!op||(!integer(t+0x24)&&b.active_transforms))break;
        if(op==0x80)b.protection=integer(t+0x10);
        else if(op==0x400)b.state=3;
        else if(op==0x100000){if(integer(t+0x10))l.body.flags=(l.body.flags&~0x70u)|0x10u;else l.body.flags&=~0x70u;l.body.th13_native_flags&=~0x100u;}
    }
    // Negative masks use a separate delayed-motion clock; refuse that
    // externally seeded state until its consumer is measured.
    if(i32(b.active_transforms)<0)return -2;
    if(b.active_transforms&&b.protection)b.protection=wrapping_sub(b.protection,1);
    if(scalar(r+0x20)>b.length){b.length+=b.speed*world.rate;if(b.length>scalar(r+0x20))b.length=scalar(r+0x20);}
    b.angle=normalize_angle(b.angle+scalar(r+0x1c)*world.rate);
    if(integer(r+0x58)&1){Vec3 boss;if(world.boss_position(boss))b.position=boss;}
    b.position.x+=scalar(r+0xc)*world.rate;b.position.y+=scalar(r+0x10)*world.rate;b.position.z+=scalar(r+0x14)*world.rate;
    switch(b.state){
    case 3:if(b.lifetime.current>=integer(r+0x30)){b.lifetime.set(0,&world.rate);b.state=4;}break;
    case 4:
        if(b.lifetime.current<integer(r+0x34)){b.width=scalar(r+0x28)*b.lifetime.fractional/float(integer(r+0x34));break;}
        b.lifetime.set(0,&world.rate);b.state=2;b.width=scalar(r+0x28);[[fallthrough]];
    case 2:if(b.lifetime.current<integer(r+0x38))break;b.lifetime.set(0,&world.rate);b.state=5;[[fallthrough]];
    case 5:if(b.lifetime.current>=integer(r+0x3c))return 1;b.width=scalar(r+0x28)-(b.lifetime.fractional*scalar(r+0x28))/float(integer(r+0x3c));break;
    default:break;
    }return 0;
}
i32 laser_th14_infinite_collision(Th14LaserInfinite& l,LaserWorld& world,bool probe){
    auto& b=l.base;
    // 43e300 passes the base position unchanged, length * .9 and this
    // narrowed width to the already measured TH14 laser collision handler.
    if((b.state==4||b.state==2)&&b.length>16.f){
        const float width=b.width<32.f?b.width*.5f:b.width-(b.width+16.f)/3.f;
        const i32 hit=probe?world.warning_collision(b.position,b.angle,width,b.length*.8999999761581421f):world.collision(b.position,b.angle,width,b.length*.8999999761581421f);
        if(hit<0){l.diagnostic=2;return -2;}
        if(hit==1&&world.th14_infinite_cut(l,world.player,{32,32},0,0,true)<0){l.diagnostic=3;return -2;}
        if(hit==2){if(b.graze_timer.current%3==0&&!world.th14_curve_graze(b.position)){l.diagnostic=4;return -2;}b.graze_timer.tick();}
    }
    return 0;
}
i32 laser_th14_infinite_update(Th14LaserInfinite& l,AnmEnvironment& animations,LaserWorld& world){
    l.diagnostic=0;const i32 step=laser_th14_infinite_step(l,world);if(step){l.diagnostic=1;return step;}
    if(laser_th14_infinite_collision(l,world)<0)return -2;auto& b=l.base;
    if(!l.body.sprite){l.diagnostic=5;return -2;}
    // 46f1e0 copies native sprite +38/+34 into VM X/Y size: width/height.
    l.body.scale.x=b.width/l.body.sprite->width;l.body.scale.y=b.length/l.body.sprite->height;l.body.flags|=8;
    if(l.body.update(animations)<0){l.diagnostic=6;return -2;}
    if(b.offset==0.f&&l.origin.update(animations)<0){l.diagnostic=7;return -2;}return 0;
}
bool laser_th14_infinite_draw(Th14LaserInfinite& l,AnmRenderer& renderer){
    l.body.script_position=l.base.position;l.body.rotation.z=normalize_angle(l.base.angle+1.5707963705062866f);l.body.flags|=4;
    if(renderer.draw(l.body)==-2)return false;
    if(l.base.offset==0.f){l.origin.script_position=l.base.position;if(renderer.draw(l.origin)==-2)return false;}return true;
}
i32 laser_th14_infinite_cancel(Th14LaserInfinite& l,bool reward,bool skip,LaserWorld& world)noexcept{
    auto& b=l.base;if(skip&&b.protection)return 0;
    const auto half=polar(b.angle,8.f);Vec3 p{b.position.x+half.x,b.position.y+half.y,b.position.z};
    const Vec2 step{half.x+half.x,half.y+half.y};float distance=8.f;i32 count=0;
    if(b.length>16.f)do{
        if(++count>1000000)return -2;
        if(visible(p,16.f)){
            const i32 family=(b.sprite<=17||b.sprite==33||b.sprite==37)?181:b.sprite<=30?229:b.sprite<=32?253:-1;
            const i32 script=family<0?-1:b.color*2+family;
            if(script>=0&&!world.cancel_effect(p,script))return -2;
            if(reward&&!world.th14_curve_reward(p,1u))return -2;
        }
        p.x+=step.x;p.y+=step.y;distance+=16.f;
    }while(b.length>distance+8.f);
    b.state=1;return count;
}
i32 laser_th14_infinite_cut_circle(Th14LaserInfinite& l,Vec3 center,float radius,u32 rewards,bool skip,std::vector<std::array<u8,0x354>>& fragments,LaserWorld& world){
    fragments.clear();auto& b=l.base;if(skip&&b.protection)return 0;if(!(b.length>16.f))return 0;if(b.length>4112.f)return -2;
    const Vec3 original=b.position;const auto half=polar(b.angle,8.f);const Vec2 step{half.x+half.x,half.y+half.y};
    Vec3 p{original.x+half.x,original.y+half.y,0};std::array<u8,256> mask{};const float squared=radius*radius;float distance=8.f;i32 count=0,selected=0;
    do{
        if(count>=256)return -2;const float dx=center.x-p.x,dy=center.y-p.y;
        if(!(dx*dx+dy*dy>squared)){
            mask[count]=1;++selected;if(rewards&&!world.th14_curve_reward(p,rewards))return -2;
            if(visible(p,32.f)){
                const i32 family=(b.sprite<=17||b.sprite==33||b.sprite==37)?181:b.sprite<=30?229:b.sprite<=32?253:-1;
                if(family>=0&&!world.cancel_effect(p,b.color*2+family))return -2;
            }
        }
        ++count;p.x+=step.x;p.y+=step.y;distance+=16.f;
    }while(b.length>distance+8.f);
    if(!selected)return 0;i32 at=0;while(at<count&&!mask[at])++at;b.length=float(at)*16.f;
    while(at<count){
        while(at<count&&mask[at])++at;const i32 begin=at;while(at<count&&!mask[at])++at;if(at==begin)break;
        const float start=float(begin);const Vec3 position{original.x+start*step.x,original.y+start*step.y,original.z};if(!visible(position,32.f))continue;
        std::array<u8,0x354> request{};
        const auto store=[&](u32 offset,const auto& value){std::memcpy(request.data()+offset,&value,4);};
        std::memcpy(request.data(),&position,12);store(0xc,b.angle);const float length=float(at-begin)*16.f;
        store(0x10,length);store(0x14,length);const float limit=scalar(l.request+0x20)-start*16.f;store(0x18,limit);store(0x1c,b.width);store(0x20,8.f);
        const i32 sprite=integer(l.request+0x50),color=integer(l.request+0x54);store(0x24,sprite);store(0x28,color);fragments.push_back(request);
    }return selected;
}
i32 laser_th14_infinite_cut_rectangle(Th14LaserInfinite& l,Vec3 center,Vec2 size,float angle,u32 rewards,bool skip,std::vector<std::array<u8,0x354>>& fragments,LaserWorld& world){
    // Retail 43e500 samples every 16 units, retaining the prefix in the
    // infinite laser and constructing kind-0 requests for remaining runs.
    fragments.clear();auto& b=l.base;if(skip&&b.protection)return 0;if(b.length<16.f)return 0;if(b.length>4096.f)return -2;
    const Vec3 original=b.position;const auto half=polar(b.angle,8.f);const Vec2 step{half.x+half.x,half.y+half.y};
    Vec3 p{original.x+half.x,original.y+half.y,0};
    const float x=original.x-center.x,y=original.y-center.y,sine=float(std::sin(-double(angle))),cosine=float(std::cos(-double(angle)));
    const auto v=polar(normalize_angle(b.angle-angle),8.f);Vec2 local{x*cosine-y*sine+v.x,y*cosine+x*sine+v.y};const Vec2 local_step{v.x+v.x,v.y+v.y};size.x*=.5f;size.y*=.5f;
    std::array<u8,256> mask{};float distance=8.f;i32 count=0,selected=0;
    do{
        if(count>=256)return -2;
        if(!(local.x< -size.x||local.x>size.x||local.y< -size.y||local.y>size.y)){
            mask[count]=1;++selected;if(rewards&&!world.th14_curve_reward(p,rewards))return -2;
            const i32 family=(b.sprite<=17||b.sprite==33||b.sprite==37)?181:b.sprite<=30?229:b.sprite<=32?253:-1;
            if(family>=0&&!world.cancel_effect(p,b.color*2+family))return -2;
        }
        ++count;p.x+=step.x;p.y+=step.y;local.x+=local_step.x;local.y+=local_step.y;distance+=16.f;
    }while(!(b.length<distance+8.f));
    if(!selected)return 0;i32 at=0;while(at<count&&!mask[at])++at;b.length=float(at)*16.f;
    while(at<count){
        while(at<count&&mask[at])++at;const i32 begin=at;while(at<count&&!mask[at])++at;if(at==begin)break;
        std::array<u8,0x354> request{};const auto store=[&](u32 offset,const auto& value){std::memcpy(request.data()+offset,&value,4);};
        const float start=float(begin),length=float(at-begin)*16.f;const Vec3 position{original.x+start*step.x,original.y+start*step.y,original.z};std::memcpy(request.data(),&position,12);
        store(0xc,b.angle);store(0x10,length);store(0x14,length);const float limit=scalar(l.request+0x20)-start*16.f;store(0x18,limit);store(0x1c,b.width);store(0x20,8.f);
        const i32 sprite=integer(l.request+0x50),color=integer(l.request+0x54),sound=-1;const u32 flags=(u32(integer(l.request+0x58))>>1)&1u;
        store(0x24,sprite);store(0x28,color);store(0x30,flags);store(0x34c,sound);store(0x350,sound);fragments.push_back(request);
    }return selected;
}
#endif
}
