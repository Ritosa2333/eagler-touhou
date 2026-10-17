#include "BulletState.hpp"
#include "Movement.hpp"
namespace th11 {
i32 bullet_start_transforms(BulletState& b,BulletEnvironment& e)noexcept {
    const auto angle=[&](float v){return v<=-990?b.angle:v>=990?bullet_aim(b.position,e.player):v;};
    const auto centered=[&](){return b.transform_index==0||b.transform_sound<0||e.sound(b.transform_sound,0,false);};
    for(u32 budget=0;b.transform_index<18;++budget){
        if(budget>=4096||b.transform_index<0)return -2;
        const auto& t=b.transforms[b.transform_index];const u32 op=t.opcode;
        if(!op||(!t.concurrent&&b.active_transforms))return 0;
        switch(op){
        case 1:b.active_transforms|=1;b.motion[0].timer.set(0,&e.rate);b.motion[0].vector.z=0;break;
        case 4:{auto& s=b.motion[1];b.active_transforms|=4;s.a=t.a;s.b=angle(t.b);s.timer.set(0,&e.rate);s.duration=t.c;const auto v=polar(s.b,s.a);s.vector.x=v.x;s.vector.y=v.y;if(!centered())return -2;break;}
        case 8:{auto& s=b.motion[2];b.active_transforms|=8;s.a=t.a;s.b=t.b;s.timer.set(0,&e.rate);s.duration=t.c;if(!centered())return -2;break;}
        case 0x10:case 0x20:case 0x40:{auto& s=b.motion[3];b.active_transforms|=op;s.b=angle(t.a);s.a=t.b<=-999?b.speed:t.b;s.timer.set(0,&e.rate);s.duration=t.c;s.limit=t.d;s.count=0;break;}
        case 0x100:{auto& s=b.motion[4];b.active_transforms|=op;s.a=t.a;s.duration=0;s.limit=t.c;s.count=t.d;break;}
        case 0x200:b.collision_delay=t.c;break;
        case 0x400:b.offscreen_grace=t.c;break;
        case 0x800:case 0x1000000:if(!e.change_appearance(b,t.c,t.d,op==0x1000000))return -2;break;
        case 0x1000:b.active_transforms|=op;b.motion[5].timer.set(t.c,&e.rate);break;
        case 0x2000:if(!e.cancel(b))return -2;break;
        case 0x4000:if(!e.sound(t.c,b.position.x,true))return -2;break;
        case 0x20000:case 0x40000:b.active_transforms|=op;b.motion[op==0x20000?6:7].timer.set(t.c,&e.rate);break;
        case 0x80000:{
            if(b.transform_index>=17)return -2;
            const auto& next=b.transforms[b.transform_index+1];const u32 packed=u32(t.c);
            BulletEmitter emitter{};emitter.position=b.position;emitter.speed=t.a;emitter.slow_speed=t.b;
            emitter.sprite=(packed>>16)&255;emitter.color=i8((packed>>8)&255);emitter.aim=(packed>>24)&127;
            emitter.transform_start=packed&255;emitter.count=i16(t.d);emitter.layers=i16(next.c);
            emitter.angle=next.a;emitter.spread=next.b;emitter.flags=u32(next.d);emitter.transform_sound=-1;
            std::memcpy(emitter.transforms,b.transforms,sizeof b.transforms);
            ++b.transform_index;if(!e.fire(emitter))return -2;++b.transform_index;
            if(!(packed&0x80000000))continue;
            if(!e.cancel(b))return -2;break;
        }
        case 0x200000:b.extra=t.c;break;
        case 0x400000:b.transform_index=t.c;continue;
        case 0x800000:{auto& s=b.motion[8];b.active_transforms|=op;s.a=t.a;s.b=t.b;s.timer.set(0,&e.rate);s.duration=t.c;break;}
        case 0x2000000:{auto& s=b.motion[9];b.active_transforms|=op;s.vector={t.a,t.b,0};
            if(t.d&0x100){s.vector.x=float(double(s.vector.x)+b.position.x);s.vector.y=float(double(b.position.y)+s.vector.y);}
            s.a=b.speed;s.duration=t.c;s.limit=t.d&255;s.timer.set(0,&e.rate);
            auto& p=b.position_interpolation;std::memcpy(p.start,&b.position,12);std::memcpy(p.end,&s.vector,12);
            std::memcpy(p.initial_tangent,&e.default_tangent,12);std::memcpy(p.final_tangent,&e.default_tangent,12);
            p.duration=t.c;p.mode=InterpolationMode(t.d&255);p.timer.set(0,&e.rate);break;
        }
        case 0x4000000:{if(t.a>=990)b.angle=normalize_angle(float(double(bullet_aim(b.position,e.player))+float(double(t.a)-999)));else if(t.a>=-990)b.angle=t.a;
            if(t.b>=-990)b.speed=t.b;const auto v=polar(b.angle,b.speed);b.velocity.x=v.x;b.velocity.y=v.y;break;}
        case 0x8000000:{auto& s=b.motion[10];b.active_transforms|=op;const auto v=polar(t.a,t.b);s.vector={v.x,v.y,0};s.b=t.a;s.a=t.b;s.duration=t.c;s.timer.set(0,&e.rate);break;}
        case 0x10000000:b.vm.flags=t.c?(b.vm.flags&~0x60u)|0x10u:b.vm.flags&~0x70u;break;
        default:break; // Original unknown transform types are skipped.
        }
        ++b.transform_index;
    }return 0;
}
}
