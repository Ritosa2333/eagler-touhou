#include "PlayerBombPatterns.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
#include "Presentation.hpp"
#include <cmath>
namespace th08 {
namespace {
Extended length(float x,float y){return number((number(x)*number(x)+number(y)*number(y)).to_float()).square_root();}
Vec3 difference(const Vec3& a,const Vec3& b){return {Scalar::sub(a.x,b.x),Scalar::sub(a.y,b.y),Scalar::sub(a.z,b.z)};}
void orbit(PlayerBombObject& object,u32 index){
    object.angle=add_angle(object.angle,index&1?.05235987901687622f:-.05235987901687622f);
    object.position.x=(sine(object.angle)*number(object.speed)+number(object.history[0].x)).to_float();
    object.position.y=(cosine(object.angle)*number(object.speed)+number(object.history[0].y)).to_float();
}
void move_regions(PlayerBombObject& o){if(o.cancel)o.cancel->position={o.position.x,o.position.y};if(o.damage)o.damage->position={o.position.x,o.position.y};}
}
void PlayerBombPatterns::reimu_begin(bool last){
    begin(last?PlayerBombKind::ReimuLast:PlayerBombKind::Reimu,0,200,260,last);
    actions.spawn_effect(12,movement.position,1,0xff4040ff);float angle=-3.1415927410125732f;
    for(u32 i=0;i<16;++i){auto& o=objects.objects[i];actions.animation(o.animation[0],19,false);o.angle=angle;angle=Scalar::add(angle,.39269909262657166f);
        o.position=o.history[0]=movement.position;o.speed=0;o.state=1;
        o.cancel=&regions.circle(false,{movement.position.x,movement.position.y},96,0,6,200);
        o.damage=&regions.circle(true,{o.position.x,o.position.y},64,0,5,200);o.damage->interval=2;o.damage->damage_limit=200;o.damage->suppress_effect=1;
    }
    if(last)bomb.sequence=0;actions.sound(13,0);
}
void PlayerBombPatterns::reimu_explode(PlayerBombObject& o,bool last){
    if(o.cancel)o.cancel->active=0;if(o.damage)o.damage->active=0;
    regions.circle(false,{movement.position.x,movement.position.y},64,4.266666889190674f,6,30);
    auto& damage=regions.circle(true,{o.position.x,o.position.y},64,last?8.533333778381348f:12.800000190734863f,last?25:500,last?15:12);
    damage.interval=last?5:4;damage.damage_limit=last?50:0;
    actions.spawn_effect(6,o.position,8,0xffffffff);o.state=2;o.animation[0].pendingInterrupt=1;
    // The original computes velocity / 8 into an unused temporary here.
    actions.panned_sound(15,o.position.x);actions.screen(ScreenEffectType::Shake,16,8,0,0,21);
}
void PlayerBombPatterns::reimu(bool last){
    const i32 time=bomb.timer.current;const bool changed=bomb.timer.changed();
    if(changed&&time==0)reimu_begin(last);
    if(last){
        for(u32 i=0;i<16;++i){auto& o=objects.objects[i];if(o.state==1){const Vec3 previous=o.position;orbit(o,i);if(time<40)o.speed=Scalar::add(o.speed,i&1?1.2f:2.4f);o.velocity=difference(o.position,previous);
                if(time>=wrapping_sub(wrapping_sub(bomb.duration,40),i))reimu_explode(o,true);move_regions(o);
            }actions.step_animation(o.animation[0]);
        }
        if(time>=40&&time%20==0){const u32 index=u32(bomb.sequence)+16;if(index>=128)return;auto& o=objects.objects[index];actions.animation(o.animation[0],20,false);
            // This is an assignment in the original: later pulses reuse slot 17.
            bomb.sequence=1;o.state=1;static constexpr u32 colors[]{0x8fffffff,0x8f0000ff,0x8fff00ff,0x8fff0000,0x8fffff00,0x8f00ff00,0x8f00ffff};const u32 color=colors[u32(time/20)%7];
            o.position=frame.homing_target.x<=-100?Vec3{(rng.range(320)+number(32)).to_float(),(rng.range(384)+number(32)).to_float(),0}:frame.homing_target;
            actions.spawn_effect(49,o.position,1,color);actions.spawn_effect(55,o.position,1,color);
            o.cancel=&regions.circle(false,{o.position.x,o.position.y},64,4.266666889190674f,6,30);
            o.damage=&regions.circle(true,{o.position.x,o.position.y},64,8.533333778381348f,400,15);o.damage->interval=2;
            actions.panned_sound(15,o.position.x);actions.screen(ScreenEffectType::Shake,16,8,0,0,21);actions.screen(ScreenEffectType::Flash,8,1,signed_bits(color),0,21);
        }
        for(u32 i=16;i<128;++i){auto& o=objects.objects[i];if(o.state){move_regions(o);if(actions.step_animation(o.animation[0]))o.state=0;}}
        return;
    }
    if(time<40){for(u32 i=0;i<16;++i){auto& o=objects.objects[i];const Vec3 previous=o.position;orbit(o,i);o.speed=Scalar::add(o.speed,3.2f);o.velocity=difference(o.position,previous);}}
    else {
        if(time==40)for(u32 i=0;i<16;++i){auto& o=objects.objects[i];o.distance=length(o.velocity.x,o.velocity.y).to_float();o.angle=Extended::from_double(std::atan2(double(o.velocity.x),double(o.velocity.y))).to_float();o.frame=0;o.speed=8;}
        for(u32 i=0;i<16;++i){auto& o=objects.objects[i];if(!o.state)continue;
            if(o.state==1&&changed){const Vec3& target=frame.homing_target.x<=-100?movement.position:frame.homing_target;
                float x=Scalar::sub(target.x,o.position.x),y=Scalar::sub(target.y,o.position.y);
                float divisor=(length(x,y)/(number(o.speed)/number(8))).to_float();if(divisor<1)divisor=1;
                x=(number(x)/number(divisor)+number(o.velocity.x)).to_float();y=(number(y)/number(divisor)+number(o.velocity.y)).to_float();const auto magnitude=length(x,y);divisor=magnitude.to_float();
                o.speed=number(10)<magnitude?10:divisor;if(o.speed<1)o.speed=1;
                o.velocity.x=(number(x)*number(o.speed)/number(divisor)).to_float();o.velocity.y=(number(y)*number(o.speed)/number(divisor)).to_float();
                regions.circle(false,{o.position.x,o.position.y},128,0,6,0);
                if((o.damage&&o.damage->damage_limit<=o.damage->damage_dealt)||time>=wrapping_sub(bomb.duration,30))reimu_explode(o,false);
            }
            o.position.x=(number(frame.timing.rate)*number(o.velocity.x)+number(o.position.x)).to_float();o.position.y=(number(frame.timing.rate)*number(o.velocity.y)+number(o.position.y)).to_float();
        }
    }
    for(u32 i=0;i<16;++i){auto& o=objects.objects[i];if(o.state){if(o.state==1)move_regions(o);else if(changed){o.frame=wrapping_add(o.frame,1);if(o.frame>29)o.state=0;}actions.step_animation(o.animation[0]);}}
}
void PlayerBombPatterns::draw_reimu(bool last,const Vec2& offset){
    tint(last?0x802020d0:0x80404040);for(u32 i=0;i<(last?128u:16u);++i){auto& o=objects.objects[i];if(!o.state)continue;auto& source=o.animation[0];AnmVm copy;if(presentation::render_only){copy=source;presentation_visual(i,0,copy);}auto& vm=presentation::render_only?copy:source;const Vec3 position=presentation_position(i);
        TH08_AUDIT_SCOPE(PlayerBomb,&o,source.currentTimeInScript.current,(u32(o.state)<<8));
        vm.pos={Scalar::add(position.x,vm.pos2.x),Scalar::add(position.y,vm.pos2.y),Scalar::add(position.z,vm.pos2.z)};
        vm.pos.x=Scalar::add(offset.x,vm.pos.x);vm.pos.y=Scalar::add(offset.y,vm.pos.y);vm.pos.z=0;actions.draw(vm,false);
    }
}
}
