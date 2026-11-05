#include "EnemyCommands.hpp"
#include "LaserManager.hpp"
#include "BulletManager.hpp"
namespace th11 {
i32 enemy_laser_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
    const u16 op=c.instruction->opcode;if(!w.lasers)return -2;auto& lasers=*w.lasers;
    auto i=[&](u32 n){return c.integer_argument(n,g);};auto f=[&](u32 n){return float(c.float_argument(n,g));};
    const auto origin=[&](){const auto& offset=e.emitter_offset[0];const auto& absolute=e.emitter_origin[0];
        if(absolute.z<=.9)return Vec3{float(double(e.current.position.x)+offset.x),float(double(e.current.position.y)+offset.y),float(double(e.current.position.z)+offset.z)};
        return Vec3{float(double(absolute.x)+offset.x),float(double(absolute.y)+offset.y),0};
    };
    switch(op){
    case 0x16d:lasers.mark_all();return 0;
    case 0x19a:
        if(!w.bullets||!w.bullets->cancel_rectangle(true,w.cancellation?*w.cancellation:BulletCancelContext{}))return -2;
        return lasers.cancel_all(true,false)?0:-2;
    case 0x1a4:case 0x1a5:case 0x1be:case 0x1bf:{
        if(!w.bullets)return -2;const float radius=f(0);const bool reward=op==0x1a4||op==0x1be;
        if(!w.bullets->cancel_circle(e.current.position,radius,reward,op>=0x1be,w.cancellation?*w.cancellation:BulletCancelContext{}))return -2;
        // All four commands preserve protected lasers, independently of the bullet delay flag.
        return lasers.cancel_circle(e.current.position,radius,reward,true)<0?-2:0;
    }
    case 0x19c:case 0x1ac:case 0x1af:case 0x1b1:{
        LaserLineParameters p{};if(op==0x1af||op==0x1b1)std::memcpy(p.transforms,e.emitters[0].transforms,sizeof p.transforms);
        p.position=origin();p.sprite=i16(i(0));p.color=i16(i(1));p.angle=f(2);
        if(op==0x19c||op==0x1af)p.angle=normalize_angle(p.angle);
        p.speed=f(3);p.initial_length=f(4);p.growth_limit=f(5);p.end_distance=f(6);p.width=f(7);
        p.start_sound=e.emitters[0].fire_sound;p.transform_sound=e.emitters[0].transform_sound;p.flags=(op==0x19c||op==0x1af)?1:0;
        lasers.spawn(p);return lasers.last_error<0?-2:0;
    }
    case 0x19d:case 0x1ad:case 0x1b0:case 0x1b2:{
        const bool additive=op==0x19d||op==0x1b0;LaserInfiniteParameters p{};p.speed=8;
        i32* destination=additive?nullptr:c.integer_reference(0,g);
        if(op==0x1b0||op==0x1b2)std::memcpy(p.transforms,e.emitters[0].transforms,sizeof p.transforms);
        p.position=origin();p.sprite=i16(i(1));p.color=i16(i(2));p.angle=f(3);if(additive)p.angle=normalize_angle(p.angle);
        p.initial_length=f(4);p.max_length=f(5);p.warning_frames=i(6);p.expand_frames=i(7);p.active_frames=i(8);p.shrink_frames=i(9);p.width=f(10);p.flags=u32(i(11));
        if(additive){p.flags|=2;p.start_sound=e.emitters[0].fire_sound;p.transform_sound=e.emitters[0].transform_sound;if(p.flags&4)p.id=i(0);else destination=c.integer_reference(0,g);}
        else{p.flags&=~2u;p.start_sound=e.emitters[0].fire_sound;p.transform_sound=e.emitters[0].transform_sound;}
        const i32 id=lasers.spawn(p);if(destination)*destination=id;return lasers.last_error<0?-2:0;
    }
    case 0x19e:case 0x19f:case 0x1a0:case 0x1a1:case 0x1a2:case 0x1a3:{
        auto* b=lasers.find(i(0));if(!b)return 0;
        if(op==0x19e||op==0x19f){const float y=f(2),x=f(1);const Vec3 value{x,y,0};if(op==0x19e)b->position=value;else std::memcpy(reinterpret_cast<u8*>(b)+sizeof(LaserState)+12,&value,sizeof value);}
        else if(op==0x1a0)b->speed=f(1);else if(op==0x1a1)b->width=f(1);else if(op==0x1a2)b->angle=f(1);
        else{const float value=f(1);std::memcpy(reinterpret_cast<u8*>(b)+sizeof(LaserState)+28,&value,sizeof value);}return 0;
    }
    case 0x1c0:{
        for(u32 budget=0;budget<=LaserManager::capacity;++budget){auto* b=lasers.find(i(0));if(!b)return 0;
            const auto result=b->type==0?laser_line_cancel(*reinterpret_cast<LaserLine*>(b),true,false,lasers):laser_infinite_cancel(*reinterpret_cast<LaserInfinite*>(b),true,false,lasers);
            if(result<0)return -2;b->id=0;
        }return -2;
    }
    default:return -2;
    }
}
}
