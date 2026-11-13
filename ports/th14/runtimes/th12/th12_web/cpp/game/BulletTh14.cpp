#include "BulletTh14.hpp"
#include "BulletManager.hpp"
#include "BulletLaunch.hpp"
#include "Movement.hpp"
#include "EnemyCommands.hpp"
#include "EnemyManager.hpp"
#include "ShotDamageTh14.hpp"
#include <cmath>
namespace th11 {
#include "Th14BulletTables.inc"
#if PORT_TITLE == 14
namespace {
i32 th14_sprite(AnmVm& vm,i32 frame){
    const auto& b=*reinterpret_cast<BulletState*>(reinterpret_cast<u8*>(&vm)-offsetof(BulletState,vm));
    if(u32(b.sprite)>=43)return -1;const auto& a=th14_bullet_appearances[b.sprite];if(a.sprites[0]<0)return frame;
    const i32 index=b.color*4+frame;return u32(index)<64?a.sprites[index]:-1;
}
void retreat(BulletState& b){b.position.x-=b.velocity.x*4.f;b.position.y-=b.velocity.y*4.f;b.position.z-=b.velocity.z*4.f;}
void move_th14(BulletState& b,float rate,bool half){
    const auto step=[&](float v){const float delta=v*rate;return half?delta*.5f:delta;};
    b.position.x+=step(b.velocity.x);b.position.y+=step(b.velocity.y);b.position.z+=step(b.velocity.z);
}
void countdown(Timer& timer){
    timer.previous=timer.current;const float rate=timer.rate?th14_rate_load(timer.rate):1.f;
    const float delta=rate>.99f&&rate<1.01f?1.f:rate;
    timer.fractional-=delta;timer.current=truncate_int(timer.fractional);
}
}
i32 bullet_th14_start_transforms(BulletState& b,BulletEnvironment& e)noexcept{
    g_th14_bullet_sprite=b.sprite;
    for(u32 n=0;n<18;++n){g_th14_bullet_program[2*n]=b.th14_transforms[n].opcode;g_th14_bullet_program[2*n+1]=b.th14_transforms[n].concurrent;}
    for(u32 budget=0;b.transform_index<18;++budget){
        if(budget>=4096||b.transform_index<0)return -2;
        const auto& t=b.th14_transforms[b.transform_index];const u32 op=t.opcode;
        g_th14_bullet_transform_index=b.transform_index;g_th14_bullet_transform_opcode=i32(op);
        if(!op||(!t.concurrent&&(b.active_transforms&~0x100u))||(op&b.active_transforms))return 0;
        switch(op){
        // 0x418280: `or ecx,1; [bullet+0xc04]=ecx`, `timer.set(0, rate)` on the object at
        // bullet+0xf40 and its vector.z zeroed.
        case 1:b.active_transforms|=1;b.motion[0].timer.set(0,&e.rate);b.motion[0].vector.z=0;break;
        case 4:{
            auto& slot=b.motion[1];b.active_transforms|=4;slot.a=t.a;
            slot.b=t.b<=-990.f?b.angle:normalize_angle(t.b>=990.f?bullet_aim(b.position,e.player)+t.c:t.b);
            slot.timer.set(0,&e.rate);slot.duration=t.p;const auto v=polar(slot.b,slot.a);slot.vector.x=v.x;slot.vector.y=v.y;
            if(b.transform_index&&b.transform_sound>=0)e.sound(b.transform_sound,b.position.x,true);break;
        }
        // 41837a: angular acceleration keeps the entry's raw a/b (no aim sentinels).
        case 8:{
            auto& slot=b.motion[2];b.active_transforms|=8;slot.a=t.a;slot.b=t.b;
            slot.timer.set(0,&e.rate);slot.duration=t.p;
            if(b.transform_index&&b.transform_sound>=0)e.sound(b.transform_sound,b.position.x,true);break;
        }
        // 418d30: the scalar increment is (target speed - current speed) / duration.
        // Unlike ETEX 4, its consumer does not replace scalar speed with vector length.
        case 0x200000:{
            auto& slot=b.motion[10];b.active_transforms|=op;slot.a=(t.a-b.speed)/float(t.p);
            slot.b=t.b<=-990.f?b.angle:normalize_angle(t.b>=990.f?bullet_aim(b.position,e.player)+t.c:t.b);
            slot.timer.set(0,&e.rate);slot.duration=t.p;const auto v=polar(slot.b,slot.a);slot.vector.x=v.x;slot.vector.y=v.y;
            if(b.transform_index&&b.transform_sound>=0)e.sound(b.transform_sound,b.position.x,true);break;
        }
        case 0x40:{
            auto& slot=b.motion[4];b.active_transforms|=op;slot.a=t.a;
            slot.vector.x=(t.q&0x20)?t.b:384.f;slot.vector.y=(t.q&0x20)?t.c:448.f;
            slot.limit=t.p;slot.count=0;slot.duration=t.q;break;
        }
        case 0x10:{
            auto& slot=b.motion[3];b.active_transforms|=op;
            switch(t.r){
            case 0:slot.b=t.a<=-990.f?b.angle:normalize_angle(t.a>=990.f?bullet_aim(b.position,e.player):t.a);break;
            case 1:case 4:slot.b=t.a;break;
            case 2:slot.b=normalize_angle(bullet_aim(b.motion[9].vector,e.player)+t.a);break;
            case 3:slot.b=normalize_angle(b.motion[9].b+t.a);break;
            case 5:{
                if(!e.th14_visual_rng)return -2;const float delta=e.th14_visual_rng->signed_unit()*t.a;
                slot.b=normalize_angle(t.a<=-990.f?b.angle+delta:t.a>=990.f?bullet_aim(b.position,e.player)+delta:delta);break;
            }
            case 6:if(!e.th14_visual_rng)return -2;slot.b=e.th14_visual_rng->signed_unit()*t.a;break;
            default:break;
            }
            slot.a=t.b>-999.f?t.b:b.speed;slot.timer.set(0,&e.rate);slot.duration=t.p;slot.limit=t.q;slot.count=0;
            b.th14_step_mode=t.r;b.th14_step_extra=t.s;break;
        }
        // 418ff1 snapshots speed, angle and XYZ for later step/move transforms.
        case 0x800000:b.motion[9].a=b.speed;b.motion[9].b=b.angle;b.motion[9].vector=b.position;break;
        case 0x400000:{
            auto& v=b.th14_size_interpolation;b.active_transforms|=op;b.flags|=0x40;
            v.start[0]=t.a;v.end[0]=t.b;v.initial_tangent[0]=v.final_tangent[0]=0;
            v.duration=t.p;v.mode=InterpolationMode(t.q);v.timer.set(0,&e.rate);break;
        }
        // 0x4181f1: `vm.interrupt(p+7)`, `word[bullet+0xc0e]=2`, then position -= velocity*[0x4c1abc].
        case 2:b.vm.interrupt(i16(t.p)+7);b.state=2;retreat(b);break;
        // 4185e2 writes the collision delay, with no active-transform bit.
        case 0x80:b.collision_delay=t.p;break;
        // 418645 resets/binds the appearance, including46f520's first ANM update.
        case 0x200:
            if(!e.change_appearance(b,t.p,t.q&0x7fff,(t.q&0x8000)!=0))return -2;
            break;
        // 418621 suppresses the disappearance effect only for p == 1,
        // then invokes the already measured retail 41ac70 with reward zero.
        case 0x400:
            if(t.p==1)b.cancel_effect=-1;
            if(!e.cancel(b))return -2;break;
        case 0x1000:{auto& slot=b.motion[5];b.active_transforms|=op;slot.count=0;slot.limit=t.p;slot.duration=t.q;break;}
        case 0x100:{auto& slot=b.motion[6];b.active_transforms|=op;slot.timer.set(t.p,&e.rate);slot.duration=t.q;break;}
        case 0x2000:{
            if(b.transform_index>=17)return -2;const auto& next=b.th14_transforms[b.transform_index+1];Th14BulletEmitter child{};
            child.sprite=next.p;child.color=next.q;child.position=b.position;child.angle=t.a<=-998.f?b.angle:normalize_angle(t.a);child.spread=t.b;
            child.speed=t.c<=-998.f?b.speed:t.c;child.slow_speed=t.d;child.count=i16(t.r);child.layers=i16(t.s);child.aim=i16(t.p);child.transform_sound=-1;child.transform_start=t.q;
            ++b.transform_index;std::memcpy(child.transforms,b.th14_transforms,sizeof child.transforms);
            if(!e.fire_th14(child))return -2;++b.transform_index;if(next.r&&!e.cancel(b))return -2;continue;
        }
        // 0x418875: `push entry.p; xmm2 = bullet.position.x; call 0x6e470`, and the machine drops the
        // result, so a missing sound is not a failure here either.
        case 0x800:e.sound(t.p,b.position.x,true);break;
        // 418a06 stores the user tag, not the animation/lifecycle state.
        case 0x8000:b.th14_script_state=t.p;break;
        // 418c5c jumps directly to p; the common increment is bypassed.
        case 0x10000:b.transform_index=t.p;continue;
        case 0x20000:{
            auto& slot=b.motion[7];b.active_transforms|=op;slot.vector={t.a,t.b,0};
            if(t.q&0x100){slot.vector.x+=b.position.x;slot.vector.y+=b.position.y;}
            slot.a=b.speed;slot.duration=t.p;slot.limit=t.q&255;slot.timer.set(0,&e.rate);
            auto& p=b.position_interpolation;std::memcpy(p.start,&b.position,12);std::memcpy(p.end,&slot.vector,12);
            std::memcpy(p.initial_tangent,&e.default_tangent,12);std::memcpy(p.final_tangent,&e.default_tangent,12);p.duration=t.p;p.mode=InterpolationMode(t.q&255);p.timer.set(0,&e.rate);break;
        }
        // Retail 418a81: >=990 is player aim plus (a-999), >=-990 is
        // an explicit normalized angle. Both scalars otherwise stay intact.
        case 0x40000:{
            if(t.a>=990.f)b.angle=normalize_angle(normalize_angle(bullet_aim(b.position,e.player)+(t.a-999.f)));
            else if(t.a>=-990.f)b.angle=normalize_angle(t.a);
            if(t.b>=-990.f)b.speed=t.b;
            const auto v=polar(b.angle,b.speed);b.velocity.x=v.x;b.velocity.y=v.y;break;
        }
        case 0x80000:{
            auto& slot=b.motion[8];b.active_transforms|=op;const auto v=polar(t.a,t.b);slot.vector={v.x,v.y,0};slot.a=t.b;slot.b=t.a;slot.duration=t.p;slot.timer.set(0,&e.rate);break;
        }
        // Retail 418f33 constructs a zeroed 84-byte descriptor, sets health
        // 10000, and invokes named ECL spawn with all eight ETEX arguments.
        case 0x1000000:{
            if(!t.script)return -2;
            EnemySpawn value{};value.position=b.position;value.health=10000;
            value.integers[0]=t.p;value.integers[1]=t.q;value.integers[2]=t.r;value.integers[3]=t.s;
            value.floats[0]=t.a;value.floats[1]=t.b;value.floats[2]=t.c;value.floats[3]=t.d;
            if(!e.th14_spawn_enemy(reinterpret_cast<const char*>(uintptr_t(t.script)),value))return -2;break;
        }
        // 419030 overrides the appearance's draw group.
        case 0x2000000:b.draw_group=t.p;break;
        case 0x80000000:
            if(t.p>0){b.active_transforms|=op;b.th14_wait_timer.set(t.p,&e.rate);}break;
        case 0x4000000:
            if(t.p>0){b.active_transforms|=op;b.th14_freeze_timer.set(t.p,&e.rate);}break;
        // 418e30 native blend bits 5..8 map to canonical bits 4..6 plus
        // th13_native_flags bit 8. Preserve canonical child/anchor fields.
        case 0x100000:
            b.vm.flags=(b.vm.flags&~0x70u)|(t.p?0x10u:0u);b.vm.th13_native_flags&=~0x100u;break;
        // The machine's own chains have no case for 0x4000 (0x418890 compares 0x1000 and 0x2000)
        // and none for most small opcodes, and every one of those paths lands on 0x4190e9, which is
        // `inc [bullet+0xc20]; jmp next`. So an unrecognised opcode advances the program; the port
        // used to return -2 here, which is what stopped the demo at frame 841.
        default:
            g_th14_bullet_unhandled|=op;
            break;
        }
        ++b.transform_index;
    }return 0;
}
i32 bullet_th14_wrap_tick(BulletState& b,BulletEnvironment& e)noexcept{
 if(!b.vm.sprite)return -2;const float width=b.vm.sprite->width,height=b.vm.sprite->height;
 const float hx=width*.5f,hy=height*.5f;auto& p=b.position;
 if(p.x+hx>-192.f&&p.x-hx<192.f&&p.y+hy>0.f&&p.y-hy<448.f)return 0;
 auto& s=b.motion[5];bool wrapped=false;
 if((s.duration&1)&&p.y<0.f){p.y=(height+448.f)+p.y;wrapped=true;}
 else if((s.duration&2)&&p.y>448.f){p.y-=height+448.f;wrapped=true;}
 else if((s.duration&4)&&p.x< -192.f){p.x=(width+384.f)+p.x;wrapped=true;}
 else if((s.duration&8)&&p.x>192.f){p.x-=width+384.f;wrapped=true;}
 if(!wrapped)return 0;s.count=wrapping_add(s.count,1);
 if(b.transform_sound>=0)e.sound(b.transform_sound,0,false);
 if(s.count>=s.limit){b.active_transforms^=0x1000u;return 1;}return 0;
}
i32 bullet_th14_offscreen_hold_tick(BulletState& b,BulletEnvironment&)noexcept{
 auto& slot=b.motion[6];countdown(slot.timer);bool complete=false;
 if(slot.duration){
  if(!b.vm.sprite)return -2;const float width=b.vm.sprite->width,height=b.vm.sprite->height,hx=width*.5f,hy=height*.5f;
  if(!(b.position.x+hx> -192.f&&b.position.x-hx<192.f&&b.position.y+hy>0.f&&b.position.y-hy<448.f)){
   const auto direction=polar(b.angle,1.f);const float left=(-384.f-width)*.5f,right=(width+384.f)*.5f,top=(-448.f-height)*.5f+224.f,bottom=(height+448.f)*.5f+224.f;
   float negative=-999.f,positive=-999.f;
   for(const auto p:std::array<Vec2,4>{Vec2{left,top},Vec2{right,top},Vec2{left,bottom},Vec2{right,bottom}}){
    const float x=p.x-b.position.x,y=p.y-b.position.y,length=std::sqrt(x*x+y*y),inverse=length>0.f?1.f/length:0.f,nx=x*inverse,ny=y*inverse;
    const float cross=direction.x*ny-direction.y*nx,dot=direction.y*ny+direction.x*nx;
    if(cross<=0.f&&dot>negative&&dot>=0.f)negative=dot;if(cross>=0.f&&dot>positive&&dot>=0.f)positive=dot;
   }
   complete=negative< -998.f||positive< -998.f;
  }
 }
 if(complete||slot.timer.current<=0){b.active_transforms^=0x100u;return 1;}return 0;
}
i32 bullet_th14_acceleration_tick(BulletState& b,BulletEnvironment& e)noexcept{
    auto& slot=b.motion[1];if(slot.timer.current>=slot.duration){b.active_transforms&=~4u;return 1;}
    b.speed+=slot.a*e.rate;b.velocity.x+=slot.vector.x*e.rate;b.velocity.y+=slot.vector.y*e.rate;b.velocity.z+=slot.vector.z*e.rate;
    constexpr float epsilon=9.999999747378752e-05f;
    if(std::abs(b.velocity.x)>epsilon||std::abs(b.velocity.y)>epsilon){
        b.angle=normalize_angle(float(std::atan2(double(b.velocity.y),double(b.velocity.x))));
        const float xx=b.velocity.x*b.velocity.x,yy=b.velocity.y*b.velocity.y;b.speed=float(std::sqrt(double(xx+yy)));
    }
    slot.timer.tick();return 0;
}
i32 bullet_th14_turn_tick(BulletState& b,BulletEnvironment& e)noexcept{
    auto& slot=b.motion[2];if(slot.timer.current>=slot.duration){b.active_transforms&=~8u;return 1;}
    // 419970 inlines two separately bounded normalizations, even for large increments.
    b.angle=normalize_angle(normalize_angle(b.angle+slot.b*e.rate));b.speed+=slot.a*e.rate;
    const auto v=polar(b.angle,b.speed);b.velocity.x=v.x;b.velocity.y=v.y;
    if(slot.timer.rate)slot.timer.tick();
    else{slot.timer.previous=slot.timer.current;slot.timer.current=wrapping_add(slot.timer.current,1);slot.timer.fractional+=1.f;}
    return 0;
}
i32 bullet_th14_speed_ramp_tick(BulletState& b,BulletEnvironment& e)noexcept{
    auto& slot=b.motion[10];if(slot.timer.current>=slot.duration){b.active_transforms&=~0x200000u;return 1;}
    b.speed+=slot.a*e.rate;b.velocity.x+=slot.vector.x*e.rate;b.velocity.y+=slot.vector.y*e.rate;b.velocity.z+=slot.vector.z*e.rate;
    constexpr float epsilon=9.999999747378752e-05f;
    if(std::abs(b.velocity.x)>epsilon||std::abs(b.velocity.y)>epsilon)
        b.angle=normalize_angle(float(std::atan2(double(b.velocity.y),double(b.velocity.x))));
    if(slot.timer.rate)slot.timer.tick();
    else{slot.timer.previous=slot.timer.current;slot.timer.current=wrapping_add(slot.timer.current,1);slot.timer.fractional+=1.f;}
    return 0;
}
i32 bullet_th14_bounce_tick(BulletState& b,BulletEnvironment& e)noexcept{
    auto& slot=b.motion[4];const auto& bounds=e.th14_bounce_bounds;
    const bool manager_width=!(bounds.x<=0.f);
    const float outer_w=manager_width?bounds.x:slot.vector.x,outer_h=manager_width?bounds.y:slot.vector.y;
    if(!(outer_w*-.5f>=b.position.x||b.position.x>=outer_w*.5f||224.f-outer_h*.5f>=b.position.y||b.position.y>=224.f+outer_h*.5f))return 0;
    const float w=manager_width?bounds.x:slot.vector.x,h=!(bounds.y<=0.f)?bounds.y:slot.vector.y;
    bool reflected=false;const bool count_only=(slot.duration&0x10)!=0;
    const auto vertical=[&](bool top){
        if(top?b.position.y<224.f-h*.5f:b.position.y>=224.f+h*.5f){
            reflected=true;if(!count_only){b.angle=normalize_angle(-b.angle);b.position.y=top?(448.f-h)-b.position.y:(448.f+h)-b.position.y;}
        }
    };
    const auto horizontal=[&](bool left){
        if(left?b.position.x<w*-.5f:b.position.x>=w*.5f){
            reflected=true;if(!count_only){
                b.angle=normalize_angle(-b.angle-3.1415927410125732422f);
                b.angle=normalize_angle(normalize_angle(b.angle+0.f));b.position.x=left?-w-b.position.x:w-b.position.x;
            }
        }
    };
    if(slot.duration&1)vertical(true);if(slot.duration&2)vertical(false);
    if(slot.duration&8)horizontal(false);if(slot.duration&4)horizontal(true);
    if(slot.a>-990.f)b.speed=slot.a;const auto v=polar(b.angle,b.speed);b.velocity.x=v.x;b.velocity.y=v.y;
    if(reflected){slot.count=wrapping_add(slot.count,1);if(b.transform_sound>=0)e.sound(b.transform_sound,b.position.x,true);}
    if(slot.count>=slot.limit){b.active_transforms&=~0x40u;return 1;}return 0;
}
i32 bullet_th14_step_tick(BulletState& b,BulletEnvironment& e)noexcept{
    auto& slot=b.motion[3];float speed=b.speed;
    if(slot.timer.current>=slot.duration){
        if(b.transform_sound>=0)e.sound(b.transform_sound,b.position.x,true);
        slot.count=wrapping_add(slot.count,1);
        switch(b.th14_step_mode){
        case 0:case 5:b.angle=normalize_angle(b.angle+slot.b);break;
        case 1:case 6:b.angle=normalize_angle(bullet_aim(b.position,e.player)+slot.b);break;
        case 2:b.angle=normalize_angle(bullet_aim(b.motion[9].vector,e.player)+slot.b);break;
        case 3:b.angle=normalize_angle(b.motion[9].b+slot.b);break;
        case 4:b.angle=normalize_angle(slot.b);break;
        default:break;
        }
        b.speed=speed=slot.a;slot.timer.set(0,&e.rate);
        if(slot.count>=slot.limit){const auto v=polar(b.angle,speed);b.velocity.x=v.x;b.velocity.y=v.y;b.active_transforms&=~0x10u;return 1;}
    }else speed-=slot.timer.fractional*speed/float(slot.duration);
    const auto v=polar(b.angle,speed);b.velocity.x=v.x;b.velocity.y=v.y;
    if(slot.timer.rate)slot.timer.tick();
    else{slot.timer.previous=slot.timer.current;slot.timer.current=wrapping_add(slot.timer.current,1);slot.timer.fractional+=1.f;}
    return 0;
}
bool BulletManager::appearance_th14(BulletState& b,i32 type,i32 color,bool interrupt,bool advance){
    if(u32(type)>=43||u32(color)>=16)return false;const auto& a=th14_bullet_appearances[type];
    if(a.secondary)return false;
    b.sprite=i16(type);b.color=i16(color);b.hitbox={a.hitbox,a.hitbox};
    if(!b.vm.bind_script(resource,a.script,file_id,&animations.rate))return false;
    b.vm.sprite_callback=th14_sprite;b.vm.reserved_430=u32(uintptr_t(&b));
    if(advance&&b.vm.update(animations)<0)return false;
    b.vm.flags2=(b.vm.flags2&~0x40000u)|0x20000u;b.flags|=0x10;
    switch(a.effect){
    case 0:b.cancel_effect=color*2+4;break;
    case 1:if(color>=8)return false;b.cancel_effect=th14_bullet_cancel_scripts[color];break;
    case 2:b.cancel_effect=-1;break;
    case 3:b.cancel_effect=16;break;
    case 4:b.cancel_effect=6;break;
    case 5:break;
    case 6:b.cancel_effect=a.sprites[color*4+3];break;
    case 7:b.cancel_effect=232;break;
    case 8:b.cancel_effect=235;break;
    case 9:b.cancel_effect=238;break;
    case 10:b.cancel_effect=247;break;
    default:break;
    }
    if(interrupt)b.vm.interrupt(2);return true;
}
void bullet_th14_size_tick(BulletState& b,BulletEnvironment& e)noexcept{
    if(!(b.active_transforms&0x400000))return;
    auto& v=b.th14_size_interpolation;const i32 mode=i32(v.mode);bool endpoint=v.duration==0;
    if(v.duration>0){
        if(v.timer.rate)v.timer.tick();else{v.timer.previous=v.timer.current;v.timer.current=wrapping_add(v.timer.current,1);v.timer.fractional+=1.f;}
        if(v.timer.current>=v.duration){v.timer.set(v.duration,&e.rate);v.duration=0;endpoint=true;}
    }
    float out;
    if(endpoint)out=(mode==7||mode==17)?v.start[0]:v.end[0];
    else if(mode==7)b.th14_size_current=out=v.start[0]+=v.end[0];
    else if(mode==17){v.start[0]+=v.final_tangent[0];v.final_tangent[0]+=v.end[0];b.th14_size_current=out=v.start[0];}
    else if(mode==8){
        const float t=v.timer.fractional/float(v.duration),m=t-1.f,r=1.f-t,twice=t*2.f;
        float result=((3.f-twice)*(t*t))*v.end[0];result+=((twice+1.f)*(m*m))*v.start[0];
        result+=((r*r)*t)*v.initial_tangent[0];result+=((m*t)*t)*v.final_tangent[0];b.th14_size_current=out=result;
    }else{const float difference=v.end[0]-v.start[0];b.th14_size_current=out=easing(v.timer.fractional,v.duration,v.mode)*difference+v.start[0];}
    b.th14_size=out;
    if(v.duration==0){b.active_transforms&=~0x400000u;if(out==1.f)b.flags&=~0x40u;}
}
bool BulletManager::cancel_th14(BulletState& b,u32 rewards){
    // 41ac70 forwards the integer reward type. Secondary templates are refused
    // by appearance_th14; only the bound primary VM exists on this path.
    b.vm.interrupt(1);
    if(!(b.flags&0x200)){
        if(b.cancel_effect>=0&&!world.effect(b.position,b.cancel_effect))return false;
        if(!world.sound(71,b.position.x,true))return false;
        if(rewards){
            const float left=b.position.x+32.f,right=b.position.x-32.f,top=b.position.y+32.f,bottom=b.position.y-32.f;
            // Native COMISS/JAE rejects ordered outside edges; NaN is unordered.
            if(!(left<=-192.f)&&!(right>=192.f)&&!(top<=0.f)&&!(bottom>=448.f)){
                ++th14_reward_count;if(!world.th14_cancel_reward(b.position,rewards,th14_reward_count))return false;
            }
        }
    }
    // 41ad1b reads the live global rate. Enemy knife slowdown can temporarily
    // change it while cancellation runs before this manager's next update.
    move_th14(b,animations.rate,true);b.state=4;b.lifetime.set(0,&rate);return true;
}
bool BulletManager::cancel_circle_th14(Vec3 center,float radius,u32 rewards,bool skip_delayed){
    // 41b2f0 / 41b3f0 walk active-list order; the latter skips +24 delays.
    for(auto* b=th14_active;b;){auto* next=b->th14_active_next;
        if((b->state==1||b->state==2)&&(!skip_delayed||!b->collision_delay)){
            const float half=b->hitbox.x*.5f,r=half+radius,rr=r*r;
            const float dx=b->position.x-center.x,dy=b->position.y-center.y;
            const float xx=dx*dx,yy=dy*dy,distance=xx+yy;
            if(skip_delayed?rr>=distance:!(rr<distance)){
                if(!cancel_th14(*b,rewards))return false;
            }
        }
        b=next;
    }
    return true;
}
bool BulletManager::cancel_all_th14(){
    // 41b880 visits all 2,000 pool slots, including already-cancelled state 4.
    // Its reward helper receives zero and returns without creating an item.
    for(u32 n=0;n<capacity;++n){auto& b=storage[n];
        if(b.state!=0&&b.state!=3&&!cancel_th14(b))return false;
    }
    return true;
}
bool BulletManager::cancel_rectangle_th14(Vec3 center,Vec2 size,float angle,u32 rewards){
    // 41b4f0 scans pool order and always skips collision-delayed bullets.
    // A second circle/rectangle test clips cancellation to the playfield.
    cancel_center={center.x,center.y};cancel_size=size;
    for(u32 n=0;n<capacity;++n){auto& b=storage[n];if((b.state!=1&&b.state!=2)||b.collision_delay)continue;
        const float radius=b.hitbox.x*b.th14_size;
        if(th14_damage_circle_rectangle(center,size,angle,b.position,radius)&&
           th14_damage_circle_rectangle({0,224,0},{384,448},0,b.position,radius)&&!cancel_th14(b,rewards))return false;
    }return true;
}
i32 BulletManager::spawn_th14(const Th14BulletEmitter& e,i32 index,i32 layer,float aim)noexcept{
    if(last_error)return -2;if(!th14_free)return 1;
    auto& b=*th14_free;th14_free=b.th14_free_next;b.th14_free_next=nullptr;
    b.th14_active_previous=nullptr;b.th14_active_next=th14_active;if(th14_active)th14_active->th14_active_previous=&b;th14_active=&b;
    BulletLaunch launch{};const bool accepted=bullet_th14_launch(e,index,layer,aim,animations.visual_rng,player,exclusion_squared,launch);
    b.position=launch.position;b.speed=launch.speed;b.angle=launch.angle;b.flags|=1;b.state=1;
    b.lifetime.set(0,&rate);b.auxiliary_timer.set(0,&rate);b.collision_delay=0;b.th14_size=1.f;
    if(!accepted){release(b);return -1;}
    b.velocity.x=launch.velocity.x;b.velocity.y=launch.velocity.y;b.flags=(b.flags&~12u)|2u;b.reserved_4c0=0;
    if(!appearance_th14(b,e.sprite,e.color,false)){last_error=74;return -2;}
    b.draw_group=th14_bullet_appearances[e.sprite].group;b.transform_sound=e.transform_sound;b.offscreen_grace=5;
    b.emitter_flags=e.flags;b.active_transforms=0;b.transform_index=e.transform_start;
    std::memcpy(b.th14_transforms,e.transforms,sizeof b.th14_transforms);
    if(u32(b.transform_index)>=18){last_error=75;return -2;}
    const auto& first=b.th14_transforms[b.transform_index];
    if(first.opcode==2){if(i16(first.p)!=1)b.vm.interrupt(i16(first.p)+7);b.state=2;retreat(b);++b.transform_index;}
    else b.vm.interrupt(2);
    if(bullet_th14_start_transforms(b,*this)<0){last_error=76;return -2;}
    if(b.vm.update(animations)<0){last_error=77;return -2;}return 0;
}
bool BulletManager::fire_th14(const Th14BulletEmitter& e){
    if(last_error||fire_depth>=64){last_error=4;return false;}++fire_depth;bool full=false;
    const float aim=bullet_aim(e.position,player);
    for(i32 layer=0;layer<e.layers&&!full;++layer)for(i32 index=0;index<e.count;++index){
        const i32 result=spawn_th14(e,index,layer,aim);if(result==-2){--fire_depth;return false;}if(result==1){full=true;break;}
    }
    --fire_depth;if((e.flags&0x20)&&!sound(e.fire_sound,e.position.x,true)){last_error=5;return false;}return true;
}
i32 BulletManager::collision_th14(BulletState& b,bool probe){
    if(!(b.flags&2))return 0;
    const i32 result=probe?world.th14_probe_collision(b):world.collision(b);if(result<0)return -2;
    if(result==1&&!b.collision_delay){b.state=3;b.vm.interrupt(1);if(b.cancel_effect>=0&&!world.th14_bullet_hit_effect(b.position,b.cancel_effect,b.velocity))return -2;}
    // Retail416f14 uses the same450410 point-value/effect/sound consumer as lasers.
    else if(result==2&&!(b.flags&4)){if(!world.th14_curve_graze(b.position))return -2;b.flags|=4;}
    return result;
}
i32 bullet_th14_position_tick(BulletState& b,BulletEnvironment& e)noexcept{
    auto& slot=b.motion[7];
    // Retail 41a4f0 snaps to the destination and restores scalar speed on
    // expiry without ticking either clock again.
    if(slot.timer.current>=slot.duration){b.active_transforms&=~0x20000u;b.position=slot.vector;b.speed=slot.a;const auto v=polar(b.angle,b.speed);b.velocity={v.x,v.y,0};return 1;}
    if(!slot.timer.current)std::memcpy(b.position_interpolation.start,&b.position,12);
    const auto p=sample(b.position_interpolation,&e.rate);b.velocity={p.x-b.position.x,p.y-b.position.y,0};
    if(std::fabs(b.velocity.x)>0.0001f||std::fabs(b.velocity.y)>0.0001f)b.angle=normalize_angle(float(std::atan2(double(b.velocity.y),double(b.velocity.x))));
    slot.timer.tick();return 0;
}
i32 bullet_th14_polar_move_tick(BulletState& b,BulletEnvironment& e)noexcept{
    auto& slot=b.motion[8];
    // Retail 41a750 clears bits 1/8 on expiry, and resets this clock to zero
    // after movement. Preserve those instructions rather than repair them.
    if(slot.timer.current>=slot.duration){b.active_transforms&=0xfffffff6u;return 1;}
    b.position.x+=slot.vector.x*e.rate;b.position.y+=slot.vector.y*e.rate;b.position.z+=slot.vector.z*e.rate;slot.timer.set(0,&e.rate);return 0;
}
i32 BulletManager::update_th14(BulletState& b)noexcept{
    b.auxiliary_timer.tick();if(b.flags&8){release(b);return -1;}
    if(b.active_transforms&~0x846a115du){last_error=78;return -2;}
    bullet_th14_size_tick(b,*this);
    if(b.state==2){
        move_th14(b,rate,true);
        if(b.lifetime.current>=8){const i32 hit=collision_th14(b);if(hit==-2)return -2;if(hit==1)goto animation;}
        if(!b.vm.integers[0])goto animation;b.state=1;
    }
    if(b.state==1){
        for(u32 budget=0;;++budget){
            if(budget>=4096||(!(b.active_transforms&0x4000000)&&bullet_th14_start_transforms(b,*this)<0)){last_error=79;return -2;}
            if(b.active_transforms&~0x846a115du){last_error=78;return -2;}
            const bool had_active=b.active_transforms!=0;bool completed=false;
            if(b.active_transforms&1){auto& timer=b.motion[0].timer;
                if(timer.current<=16){const float boost=5.f-(timer.fractional*5.f)*.0625f;const auto v=polar(b.angle,boost+b.speed);b.velocity.x=v.x;b.velocity.y=v.y;timer.tick();}
                else{b.active_transforms^=1;completed=true;}
            }
            if(b.active_transforms&4)completed=bullet_th14_acceleration_tick(b,*this)!=0||completed;
            if(b.active_transforms&0x200000)completed=bullet_th14_speed_ramp_tick(b,*this)!=0||completed;
            if(b.active_transforms&8)completed=bullet_th14_turn_tick(b,*this)!=0||completed;
            if(b.active_transforms&0x10)completed=bullet_th14_step_tick(b,*this)!=0||completed;
            if(b.active_transforms&0x40)completed=bullet_th14_bounce_tick(b,*this)!=0||completed;
            if(b.active_transforms&0x20000)completed=bullet_th14_position_tick(b,*this)!=0||completed;
            if(b.active_transforms&0x80000)completed=bullet_th14_polar_move_tick(b,*this)!=0||completed;
            if(b.active_transforms&0x100){const i32 result=bullet_th14_offscreen_hold_tick(b,*this);if(result<0){last_error=82;return -2;}completed=result!=0||completed;}
            // Native 41695d checks expiry before decrementing. Waiting keeps movement;
            // 4169ed freezes movement/ANM and prevents dispatch until expiry.
            if(b.active_transforms&0x80000000){
                if(b.th14_wait_timer.current<=0){b.active_transforms^=0x80000000;completed=true;}
                else countdown(b.th14_wait_timer);
            }
            if(b.active_transforms&0x4000000){
                if(b.th14_freeze_timer.current<=0){b.flags&=~0x200u;b.active_transforms^=0x4000000;completed=true;}
                else{b.flags|=0x200;countdown(b.th14_freeze_timer);}
            }
            if(had_active&&b.collision_delay)--b.collision_delay;
            if(!completed)break;
        }
        if(!(b.flags&0x200)){move_th14(b,rate,false);if(collision_th14(b)==-2)return -2;}
    }else if(b.state==3)move_th14(b,rate,true);
    else if(b.state!=2&&b.state!=4){last_error=80;return -2;}
animation:
    if(b.vm.sprite&&(b.active_transforms&0x1000)&&bullet_th14_wrap_tick(b,*this)<0){last_error=81;return -2;}
    if(b.vm.sprite&&!(b.active_transforms&0x100)&&b.offscreen_grace<1){
        const float hx=(b.vm.sprite->width*b.th14_size)*.5f,hy=(b.vm.sprite->height*b.th14_size)*.5f;
        if(b.position.x+hx<=-192.f||b.position.x-hx>=192.f||b.position.y+hy<=-64.f||b.position.y-hy>=448.f){release(b);return -1;}
    }
    if(b.collision_delay)--b.collision_delay;if(b.offscreen_grace>0)--b.offscreen_grace;
    if(!(b.flags&0x200)){const i32 result=b.vm.update(animations);if(result<0)return -2;if(result){release(b);return -1;}}
    return 0;
}
#endif
}
