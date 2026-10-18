#include "PlayerBombPatterns.hpp"
namespace th08 {
void PlayerBombPatterns::alice(bool last){
    if(!frame.options)return;auto& option=frame.options[0];auto& object=objects.objects[0];const i32 time=bomb.timer.current;const bool changed=bomb.timer.changed();
    if(changed&&time==0){begin(last?PlayerBombKind::AliceLast:PlayerBombKind::Alice,1,last?230:210,last?280:250,last);actions.sound(13,0);object.position=last?movement.position:option.position;if(!last)actions.sound(6,0);}
    if(time<60){
        const auto fraction=bomb.timer.value()/number(60);const float factor=(fraction*number(fraction.to_float())).to_float();
        const Vec3 delta{Scalar::sub(192,object.position.x),Scalar::sub(224,object.position.y),(-number(object.position.z)).to_float()};
        const Vec3 scaled{Scalar::mul(delta.x,factor),Scalar::mul(delta.y,factor),Scalar::mul(delta.z,factor)};
        option.position={Scalar::add(scaled.x,object.position.x),Scalar::add(scaled.y,object.position.y),Scalar::add(scaled.z,object.position.z)};
        option.animation.rotation.z=Scalar::add(option.animation.rotation.z,-.3141592741012573f);
        regions.circle(false,{option.position.x,option.position.y},32,0,6,0);regions.circle(true,{option.position.x,option.position.y},32,0,40,0);return;
    }
    option.animation.rotation.z=0;option.position.x=192;option.position.y=224;if(time>=(last?128:150))option.animation.color1.a=0;
    if(!changed)return;
    if(time>=60&&time<=76&&time%4==0){static constexpr u32 colors[]{0xffffffff,0xffffd0d0,0xffffb0b0,0xffff8080,0xffff4040};actions.spawn_effect(40,option.position,1,colors[(time-60)/4]);}
    else if(time==(last?120:90)){
        actions.sound(15,0);for(i32 effect=42;effect<=44;++effect)actions.spawn_effect(effect,option.position,1,0xffffffff);
        if(last){actions.spawn_effect(45,{64,96,0},1,0xff0000f0);actions.spawn_effect(45,{64,352,0},1,0xfff00000);actions.spawn_effect(45,{320,352,0},1,0xff00f000);actions.spawn_effect(45,{320,96,0},1,0xff00f0f0);}
        regions.circle(false,{option.position.x,option.position.y},1,5,6,110);regions.circle(true,{option.position.x,option.position.y},1,5,70,110).interval=5;
        if(!last){actions.screen(ScreenEffectType::Shake,24,8,0,0,21);actions.screen(ScreenEffectType::Flash,8,1,signed_bits(0x8fffffff),0,21);}
    }
    else if(time>=(last?130:100)&&time<=(last?160:130)&&time%10==0){static constexpr u32 colors[]{0xffffffff,0xffffd0d0,0xffff8080,0xffff0000};actions.spawn_effect(45,option.position,1,colors[(time-(last?130:100))/10]);}
    else if(time==(last?180:150)){actions.screen(ScreenEffectType::Flash,8,1,last?-1:signed_bits(0x8fffffff),0,21);actions.screen(ScreenEffectType::Shake,24,8,0,0,21);actions.sound(25,0);}
    else if(time==(last?229:209)){option.state=1;option.timer.set(0);}
}
void PlayerBombPatterns::draw_alice(){
    const i32 time=bomb.timer.current;if(time<90||time>220){tint(0x802020d0);return;}
    if(time<=120){const i32 component=signed_bits(u32(wrapping_sub(time,90))*208)/30;const u32 color=0x80000000|(u32(u8(component/5+208))<<16)|(u32(u8(component+32))<<8)|u8(component+32);tint(color);
        const u32 alpha=u8(signed_bits(u32(wrapping_sub(time,90))*255)/30);actions.rectangle(32,16,416,464,(alpha<<24)|0xffffff);
    }else actions.rectangle(32,16,416,464,0x70ffffff);
}
}
