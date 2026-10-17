#include "PlayerBombPatterns.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
#include "Presentation.hpp"
#include <cmath>
namespace th08 {
namespace {void release_regions(PlayerBombObject& o){if(o.damage)o.damage->active=0;if(o.cancel)o.cancel->active=0;o.damage=o.cancel=nullptr;}}
void PlayerBombPatterns::sakuya(bool last){
    const i32 time=bomb.timer.current;const bool changed=bomb.timer.changed();const u32 count=last?128:96;
    if(changed&&time==0){begin(last?PlayerBombKind::SakuyaLast:PlayerBombKind::Sakuya,0,last?320:250,last?350:290,last);
        for(u32 i=0;i<count;++i)objects.objects[i].state=0;movement.multiplier={.5f,.5f};objects.objects[0].effect=actions.spawn_effect(20,movement.position,1,0xffffffff);actions.sound(5,0);
        if(last)actions.screen(ScreenEffectType::Shake,50,4,1,0,21);
    }
    if(time>=0&&time<=60&&objects.objects[0].effect)objects.objects[0].effect->position=movement.position;
    if(time>=20&&time<(last?148:116)){
        for(u32 i=0;i<count;++i){auto& o=objects.objects[i];if(!changed||time!=i32(i%(last?64:48))*2+20)continue;if(o.state)return;
            o.state=1;actions.animation(o.animation[0],last?20:22,false);
            o.angle=(Extended::from_int(i)*number(6.2831854820251465f)/number(last?64:96)-number(3.1415927410125732f)).to_float();if(last)o.angle=add_angle(o.angle,0);
            o.distance=(rng.range(1)+number(.5f)).to_float();o.speed=(rng.range(.1f)+number(.03f)).to_float();o.acceleration.x=rng.bounded16(1)? .15707963705062866f:-.15707963705062866f;
            o.velocity.x=(cosine(o.angle)*number(24)).to_float();o.velocity.y=(sine(o.angle)*number(24)).to_float();
            o.position={Scalar::add(movement.position.x,o.velocity.x),Scalar::add(movement.position.y,o.velocity.y),Scalar::add(movement.position.z,o.velocity.z)};
            o.timer.set(0);o.velocity.z=0;o.cancel=&regions.circle(false,{o.position.x,o.position.y},32,0,6,500);o.damage=&regions.circle(true,{o.position.x,o.position.y},32,0,last?30:20,500);
        }
        actions.sound(6,0);if(!last)actions.screen(ScreenEffectType::Shake,120,4,1,0,21);
    }
    for(u32 i=0;i<count;++i){auto& o=objects.objects[i];if(!o.state)continue;
        if(o.timer.current<30||o.timer.current>=70){
            if(o.timer.changed()&&o.timer.current==70){if(frame.homing_target.x>-100){const float x=Scalar::sub(frame.homing_target.x,o.position.x),y=Scalar::sub(frame.homing_target.y,o.position.y);o.angle=add_angle(Extended::from_double(std::atan2(double(y),double(x))).to_float(),0);}o.distance=14;if(last)actions.spawn_effect(46,o.position,1,0xffffffff);}
            o.distance=Scalar::add(o.distance,o.speed);o.velocity.x=(cosine(o.angle)*number(o.distance)).to_float();o.velocity.y=(sine(o.angle)*number(o.distance)).to_float();
        }else{o.angle=add_angle(o.angle,o.acceleration.x);o.velocity.x=o.velocity.y=0;}
        if(o.damage){o.damage->position={o.position.x,o.position.y};if(o.cancel)o.cancel->position={o.position.x,o.position.y};
            if(o.timer.current>=120)release_regions(o);
            else if(o.damage->damage_dealt>0){
                if(last){if(i%3==0)actions.screen(ScreenEffectType::Flash,2,1,signed_bits(0x208080ff),0,21);actions.spawn_effect(0,o.position,1,0xffff80ff);actions.animation(o.animation[0],21,false);}
                else{actions.animation(o.animation[0],23,false);actions.spawn_effect(0,o.position,1,0xffff80ff);}
                release_regions(o);actions.panned_sound(43,o.position.x);
            }
        }
        o.position={Scalar::add(o.position.x,o.velocity.x),Scalar::add(o.position.y,o.velocity.y),Scalar::add(o.position.z,o.velocity.z)};actions.step_animation(o.animation[0]);o.timer.tick(frame.timing);
    }
}
void PlayerBombPatterns::draw_sakuya(bool last,const Vec2& offset){
    tint(last?0x80202080:0x80404040);
    // The last-spell update owns 128 knives; its original draw loop visits 96.
    for(u32 i=0;i<96;++i){auto& o=objects.objects[i];if(!o.state)continue;auto& source=o.animation[0];AnmVm copy;if(presentation::render_only){copy=source;presentation_visual(i,0,copy);}auto& vm=presentation::render_only?copy:source;vm.rotation.z=presentation_angle(i);vm.updateRotation=1;vm.pos=presentation_position(i);
        TH08_AUDIT_SCOPE(PlayerBomb,&o,source.currentTimeInScript.current,(u32(o.state)<<8));
        vm.pos.x=Scalar::add(offset.x,vm.pos.x);vm.pos.y=Scalar::add(offset.y,vm.pos.y);vm.pos.z=0;actions.draw(vm,true);
    }
}
}
