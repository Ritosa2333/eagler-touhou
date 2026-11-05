#include "PlayerBombPatterns.hpp"
namespace th08 {
namespace {
Vec3 approach(const Vec3& from,const Vec3& to,Extended time,i32 duration){
    const float fraction=(time/number(duration)).to_float();const auto inverse=number(1)-number(fraction);const float square=(inverse*number(inverse.to_float())).to_float();const float amount=Scalar::sub(1,square);
    const Vec3 delta{Scalar::sub(to.x,from.x),Scalar::sub(to.y,from.y),Scalar::sub(to.z,from.z)};
    const Vec3 scaled{Scalar::mul(delta.x,amount),Scalar::mul(delta.y,amount),Scalar::mul(delta.z,amount)};
    return {Scalar::add(scaled.x,from.x),Scalar::add(scaled.y,from.y),Scalar::add(scaled.z,from.z)};
}
}
void PlayerBombPatterns::youmu(bool last){
    if(!frame.main_animation)return;const i32 time=bomb.timer.current;const bool changed=bomb.timer.changed();auto& o=objects.objects[0];const u32 color=last?0xffff8080:0xff8080ff;
    if(changed&&time==0){begin(last?PlayerBombKind::YoumuLast:PlayerBombKind::Youmu,0,last?250:220,last?300:270,0);actions.sound(13,0);movement.multiplier={0,0};actions.animation(*frame.main_animation,0,false);actions.sound(6,0);o.position=movement.position;o.velocity=o.position;o.velocity.y=416;}
    if(time<40){movement.position=approach(o.position,o.velocity,bomb.timer.value(),40);return;}
    if(changed&&time==40){actions.spawn_effect(40,movement.position,1,color);return;}
    if(changed&&time==70){movement.position.y=32;const Vec3 position{movement.position.x,224,movement.position.z};actions.screen(ScreenEffectType::Flash,8,1,signed_bits(0xefffffff),0,21);actions.sound(42,0);
        o.cancel=&regions.rectangle(false,{position.x,position.y},96,448,6,60);o.damage=&regions.rectangle(true,{position.x,position.y},96,448,last?500:300,last?0:10);
        o.damage=&regions.rectangle(true,{position.x,position.y},96,448,last?100:80,60);o.damage->interval=5;actions.spawn_effect(48,position,1,color);return;
    }
    if(changed&&time>=80&&time<=(last?130:100)&&time%10==0){const i32 index=(time-80)/10;static constexpr i32 damage[]{100,100,80,60,50,40};static constexpr u32 regular[]{0xcfffffff,0xbfffffff,0x8fffffff},special[]{0xcfffffff,0xafffffff,0x8fffffff,0x6fffffff,0x5fffffff,0x5fffffff};
        const float distance=float((index+1)*32);Vec3 position{Scalar::sub(movement.position.x,distance),224,movement.position.z};
        for(i32 side=0;side<2;++side){if(side)position.x=(number(position.x)+number(distance)*number(2)).to_float();regions.rectangle(false,{position.x,position.y},96,448,6,60);o.damage=&regions.rectangle(true,{position.x,position.y},96,448,damage[index],40);o.damage->interval=side?3:2;actions.spawn_effect(48,position,1,color);}
        actions.screen(ScreenEffectType::Flash,8,1,signed_bits(last?special[index]:regular[index]),0,21);if(time==(last?130:100))o.velocity=movement.position;return;
    }
    const i32 start=last?150:120,end=last?180:150;
    if(time>=start&&time<end){movement.position=approach(o.velocity,o.position,bomb.timer.value()-number(start),30);return;}
    if(changed&&time==end)movement.multiplier={1,1};
}
void PlayerBombPatterns::draw_youmu(bool last){
    tint(0x80404040);const i32 time=bomb.timer.current;if(time<70)return;tint(0x80000030);if(time>=160)return;
    const u32 alpha=time<100?255:u8(255-signed_bits(u32(wrapping_sub(time,100))*255)/60);actions.rectangle(32,16,416,464,(alpha<<24)|(last?0xff0000:0xffffff));
}
}
