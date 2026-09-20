#include "PlayerBombPatterns.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
#include "Presentation.hpp"
#include <cmath>
namespace th08 {
namespace {float raw(u32 bits){float value;std::memcpy(&value,&bits,4);return value;}}
void PlayerBombPatterns::yuyuko(bool last){
    const i32 time=bomb.timer.current;const bool changed=bomb.timer.changed();
    if(changed&&time==0){begin(last?PlayerBombKind::YuyukoLast:PlayerBombKind::Yuyuko,1,300,350,last);
        for(auto& o:objects.objects)o.state=0;
        for(u32 group=0;group<4;++group)for(u32 i=0;i<16;++i){auto& o=objects.objects[group*16+i];o.state=1;actions.animation(o.animation[0],group&1?19:18,false);
            o.angle=(Extended::from_int(i)*number(3.1415927410125732f)/number(8)-number(3.1415927410125732f)).to_float();static constexpr u32 turns[]{0x3c567750,0xbc567750,0x3c80adfd,0xbc80adfd};o.speed=raw(turns[group]);o.position=o.history[0]=movement.position;
            o.damage=&regions.circle(true,{o.position.x,o.position.y},24,0,50,500);o.cancel=&regions.circle(false,{o.position.x,o.position.y},24,0,6,500);o.damage->damage_limit=800;o.velocity.x=0;o.velocity.y=group<2?2:1.5f;
        }
        movement.multiplier={.8f,.8f};objects.objects[0].effect=actions.spawn_effect(20,movement.position,1,0xffffffff);actions.sound(5,0);actions.screen(ScreenEffectType::Shake,last?60:120,last?16:12,0,0,21);
    }
    if(last&&time>=60&&time<200&&changed&&time%20==0){const float angle=rng.signed_range(3.1415927410125732f).to_float();u32 count=0,index=0;
        for(;index<128;++index){auto& o=objects.objects[index];if(o.state)continue;o.state=1;actions.animation(o.animation[0],20,false);o.angle=add_angle((Extended::from_int(index)*number(3.1415927410125732f)/number(8)).to_float(),angle);o.position=o.history[0]=movement.position;
            o.damage=&regions.circle(true,{o.position.x,o.position.y},64,0,100,500);o.damage->damage_limit=1200;o.cancel=&regions.circle(false,{o.position.x,o.position.y},64,0,6,500);o.velocity.x=0;o.velocity.y=8;if(++count>=16)break;
        }
        // With a full pool the original pointer reaches the immediately adjacent
        // origin and damage storage; its X read aliases the first area's radius.
        actions.panned_sound(15,index<128?objects.objects[index].position.x:regions.damaging[0].radius);
        actions.screen(ScreenEffectType::Shake,30,8,0,0,21);actions.screen(ScreenEffectType::Flash,8,1,signed_bits(0xe0f0f0f0),0,21);
    }
    for(auto& o:objects.objects){if(!o.state)continue;o.history[1]=o.position;o.velocity.x=Scalar::add(o.velocity.x,o.velocity.y);o.angle=add_angle(o.angle,o.speed);
        o.position.x=(cosine(o.angle)*number(o.velocity.x)).to_float();o.position.y=(sine(o.angle)*number(o.velocity.x)).to_float();
        o.position={Scalar::add(o.position.x,o.history[0].x),Scalar::add(o.position.y,o.history[0].y),Scalar::add(o.position.z,o.history[0].z)};
        o.history[1]={Scalar::sub(o.position.x,o.history[1].x),Scalar::sub(o.position.y,o.history[1].y),Scalar::sub(o.position.z,o.history[1].z)};
        bool done=o.velocity.x>=500;if(!done){if(o.damage){o.damage->position={o.position.x,o.position.y};if(o.cancel)o.cancel->position={o.position.x,o.position.y};}done=actions.step_animation(o.animation[0]);}
        if(done){o.state=0;if(o.damage)o.damage->active=0;if(o.cancel)o.cancel->active=0;}
    }
}
void PlayerBombPatterns::draw_yuyuko(bool last,const Vec2& offset){
    tint(last?0x80802020:0x80404040);for(u32 i=0;i<(last?128u:96u);++i){auto& o=objects.objects[i];if(!o.state)continue;auto& source=o.animation[0];AnmVm copy;if(presentation::render_only){copy=source;presentation_visual(i,0,copy);}auto& vm=presentation::render_only?copy:source;vm.rotation.z=Extended::from_double(std::atan2(double(o.history[1].y),double(o.history[1].x))).to_float();vm.updateRotation=1;vm.pos=presentation_position(i);
        TH08_AUDIT_SCOPE(PlayerBomb,&o,source.currentTimeInScript.current,(u32(o.state)<<8));
        vm.pos.x=Scalar::add(offset.x,vm.pos.x);vm.pos.y=Scalar::add(offset.y,vm.pos.y);vm.pos.z=0;actions.draw(vm,true);
    }
}
}
