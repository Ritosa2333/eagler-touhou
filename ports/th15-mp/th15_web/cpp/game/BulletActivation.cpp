#include "BulletState.hpp"
#include "AnmVisualState.hpp"
#include "LaserScene.hpp"
#include "EnemySpawn.hpp"
#include <cmath>
namespace th15 {
namespace {
Vec3 polar(float angle,float speed){return {float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed)),0};}
}
bool BulletState::activate(const Vec3& player,Rng& random,BulletSoundHost* sounds,BulletAnimationHost* animations,std::string& error){
    auto resolve_angle=[&](float value,float offset,float aiming_threshold=999990.f){if(value<=-999990.f)return motion.angle;if(value>=aiming_threshold)value=float(bullet_aim(motion.position,player)+offset);return normalize_angle(value);};
    for(u32 budget=0;budget<1024;budget++){
        if(transform_index>=18)return true;if(transform_index<0){error="Negative bullet transform index";return false;}
        const auto& t=transforms[transform_index];const auto& f=t.floats;const auto& i=t.integers;const u32 type=t.type;
        if(!type||(!t.active&&(transform_flags&0xfffffeffu))||(type&transform_flags))return true;
        switch(type){
            case 1:transform_flags|=1;boost.set(0);boost_stage=0;break;
            case 2:if(!animations||!animations->interrupt(wrapping_add(i32(i16(i[0])),7))){error="Bullet animation interrupt unavailable";return false;}phase=2;motion.position={float(motion.position.x-float(motion.velocity.x*4.f)),float(motion.position.y-float(motion.velocity.y*4.f)),float(motion.position.z-float(motion.velocity.z*4.f))};break;
            case 4:{transform_flags|=4;acceleration.speed=f[0];acceleration.angle=resolve_angle(f[1],f[2]);acceleration.timer.set(0);acceleration.duration=i[0];const auto v=polar(acceleration.angle,acceleration.speed);acceleration.vector.x=v.x;acceleration.vector.y=v.y;if(transform_index&&transform_sound>=0&&sounds)sounds->play(transform_sound);break;}
            case 8:transform_flags|=8;angular.speed=f[0];angular.angle=f[1];angular.timer.set(0);angular.duration=i[0];if(transform_index&&transform_sound>=0&&sounds)sounds->play(transform_sound);break;
            case 16:{
                transform_flags|=16;turn.speed=f[1]<=-999990.f?motion.speed:f[1];
                switch(i[2]){
                    case 0:turn.angle=resolve_angle(f[0],f[2]);break;
                    case 1:case 4:turn.angle=f[0];break;
                    case 2:turn.angle=normalize_angle(float(bullet_aim(saved.position,player)+f[0]));break;
                    case 3:turn.angle=normalize_angle(float(saved.angle+f[0]));break;
                    case 5:case 6:turn.angle=float(random.signed_unit()*f[0]);break;
                    case 7:turn.angle=resolve_angle(f[0],0,990.f);turn.speed=float(float(random.signed_unit()*f[1])+motion.speed);break;
                    default:break;
                }
                turn.timer.set(0);turn.duration=i[0];turn.limit=i[1];turn.count=0;turn.mode=i[2];turn.extra=i[3];break;
            }
            case 64:transform_flags|=64;reflection.speed=f[0];reflection.bounds=(u32(i[1])&32)?Vec2{f[1],f[2]}:Vec2{384,448};reflection.limit=i[0];reflection.count=0;reflection.sides=u32(i[1]);break;
            case 128:collision_delay=i[0];break;
            case 256:transform_flags|=256;wait.set(i[0]);wait_outside=i[1];break;
            case 512:if(!animations||!appearance(i[0],i[1],*animations,true,error))return false;break;
            case 1024:if(i[0]==1)cancellation_script=-1;if(!animations||!animations->cancel_bullet(0)){error="Bullet cancellation host unavailable";return false;}break;
            case 2048:if(sounds)sounds->play(i[0]);break;
            case 4096:transform_flags|=4096;wrapping.limit=i[0];wrapping.count=0;wrapping.sides=u32(i[1]);break;
            case 8192:{
                if(transform_index+1>=18){error="Truncated bullet split transform";return false;}
                const auto secondary=transforms[transform_index+1];BulletShooter children;children.position=motion.position;children.pattern=i16(i[0]);children.tail[0]=u32(i[1]);children.count=i16(i[2]);children.rows=i16(i[3]);children.transform_sound=-1;
                children.angle=f[0]<=-999990.f?motion.angle:normalize_angle(f[0]>=999990.f?bullet_aim(motion.position,player):f[0]);children.angle_step=f[1];children.speed=f[2]<=-999990.f?motion.speed:f[2];children.speed_step=f[3];children.sprite=secondary.integers[0];children.color=secondary.integers[1];children.transforms=transforms;
                transform_index=wrapping_add(transform_index,1);if(!animations||!animations->emit_children(children)){error="Bullet split emission unavailable";return false;}
                transform_index=wrapping_add(transform_index,1);if(secondary.integers[2]&&!animations->cancel_bullet(0)){error="Bullet split cancellation unavailable";return false;}continue;
            }
            case 32768:step_limit=i[0];break;
            case 65536:transform_index=i[0];continue;
            case 131072:{transform_flags|=131072;auto& curve=position_curve;curve.target={f[0],f[1],0};if(u32(i[1])&256){curve.target.x=float(curve.target.x+motion.position.x);curve.target.y=float(motion.position.y+curve.target.y);}curve.speed=motion.speed;curve.duration=i[0];curve.mode=u8(i[1]);curve.timer.set(0);curve.interpolation.begin(i[0],curve.mode,{motion.position.x,motion.position.y,motion.position.z},{curve.target.x,curve.target.y,0});curve.interpolation.control1=curve.interpolation.control2={};break;}
            case 262144:if(f[0]>=990.f)motion.angle=normalize_angle(normalize_angle(float(bullet_aim(motion.position,player)+float(f[0]-999.f))));else if(f[0]>=-990.f)motion.angle=normalize_angle(f[0]);if(f[1]>=-990.f)motion.speed=f[1];velocity(motion.speed,motion.angle);break;
            case 524288:transform_flags|=524288;drift.velocity=polar(f[0],f[1]);drift.angle=f[0];drift.speed=f[1];drift.duration=i[0];drift.timer.set(0);break;
            case 1048576:visual_flags=i[0]?(visual_flags&0xfffffe3fu)|32u:visual_flags&0xfffffe1fu;break;
            case 2097152:{transform_flags|=2097152;approach.speed=float(float(f[0]-motion.speed)/float(i[0]));approach.angle=resolve_angle(f[1],f[2]);approach.timer.set(0);approach.duration=i[0];const auto v=polar(approach.angle,approach.speed);approach.vector.x=v.x;approach.vector.y=v.y;if(transform_index&&transform_sound>=0&&sounds)sounds->play(transform_sound);break;}
            case 4194304:transform_flags|=4194304;scale_curve.begin(i[0],i[1],{f[0]},{f[1]});scale_curve.control1=scale_curve.control2={};flags|=64;break;
            case 8388608:saved.position=motion.position;saved.angle=motion.angle;saved.speed=motion.speed;break;
            case 33554432:cancel_item=i[0];break;
            case 67108864:if(i[0]>0){transform_flags|=67108864;freeze.set(i[0]);}break;
            case 2147483648u:if(i[0]>0){transform_flags|=2147483648u;invulnerability.set(i[0]);}break;
            case 16777216:{
                if(t.payload.empty()||t.payload.back()!=0){error="Enemy bullet transform routine unavailable";return false;}
                EnemySpawnRequest request;request.routine=reinterpret_cast<const char*>(t.payload.data());request.position=motion.position;request.life=10000;request.integers=i;request.floats=f;
                if(!animations||!animations->spawn_enemy(request)){error="Bullet enemy creation failed";return false;}break;
            }
            case 134217728:{
                if(transform_index+1>=18){error="Truncated bullet laser transform";return false;}
                const auto& next=transforms[transform_index+1];bool cancelled=false;
                if(i[0]==0){MovingLaserRequest request;request.position=motion.position;request.angle=resolve_angle(f[0],0);request.speed=f[1]<=-999990.f?motion.speed:f[1];request.initial_length=f[2];request.maximum_length=f[3];request.type=i[1];request.color=i[2];request.end_distance=next.floats[0];request.width=next.floats[1];request.start_offset=next.floats[2];request.sound=next.integers[0];request.reflection_sound=next.integers[1];request.transform_index=next.integers[2];request.transform_parameter=1;request.transforms=transforms;
                    transform_index=wrapping_add(transform_index,1);if(!animations||!animations->create_laser(request)){error="Bullet moving laser creation failed";return false;}cancelled=i[3]!=0;
                }else if(i[0]==1){StationaryLaserRequest request;request.position=motion.position;request.angle=resolve_angle(f[0],0);request.speed=f[1]<=-999990.f?motion.speed:f[1];request.initial_length=f[2];request.maximum_length=f[3];request.type=i[1];request.color=i[2];const u32 packed=u32(i[3]);request.flags=(packed&0xfdu)|2u;request.transform_index=(packed>>8)&255;request.width=next.floats[0];request.start_offset=next.floats[1];request.delay=next.integers[0];request.warmup=next.integers[1];request.active=next.integers[2];request.fade=next.integers[3];request.sound=18;request.transform_sound=-1;request.transforms=transforms;
                    transform_index=wrapping_add(transform_index,1);if(!animations||!animations->create_laser(request)){error="Bullet stationary laser creation failed";return false;}cancelled=(packed>>16)&1;
                }else{error="Invalid bullet laser kind";return false;}
                transform_index=wrapping_add(transform_index,1);if(cancelled&&(!animations||!animations->cancel_bullet(0))){error="Bullet laser conversion cancellation failed";return false;}continue;
            }
            default:break;
        }
        transform_index=wrapping_add(transform_index,1);
    }
    error="Bullet transform jump cycle exceeded original frame budget";return false;
}
}
