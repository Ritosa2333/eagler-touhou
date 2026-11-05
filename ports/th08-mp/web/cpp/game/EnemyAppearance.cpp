#include "EnemyAppearance.hpp"
namespace th08 {
void update_enemy_form(EclVm& enemy,bool youkai,EnemyAppearanceActions& actions){
    if(!(enemy.flags&0x800)){
        if(youkai){actions.effect(31,enemy.resolved_position,1,0x80303080);if(enemy.familiar_effect)enemy.familiar_effect->pendingInterrupt=2;actions.sound(40,0);enemy.draw_layer=0;}
        if((enemy.flags2&2)&&enemy.lifetime.changed()&&enemy.lifetime.current%2==0)actions.effect(38,enemy.resolved_position,1,0xffffffff);
    }else if(!youkai){actions.effect(30,enemy.resolved_position,1,0x80803030);if(enemy.familiar_effect)enemy.familiar_effect->pendingInterrupt=1;actions.sound(39,0);enemy.draw_layer=2;}
    enemy.flags=(enemy.flags&~0x800u)|(u32(youkai)<<11);enemy.difficulty_flags=youkai?0x40:0x20;
}
void update_enemy_hit_flash(EclVm& enemy,bool hit,EnemyAppearanceActions& actions){
    auto& vm=enemy.animation[0];
    if(!(enemy.flags&0x800)){
        if(enemy.hit_flash){--enemy.hit_flash;vm.flag17=false;}
        else if(hit){actions.panned_sound(((enemy.flags2>>4)&3)<2?20:37,enemy.resolved_position.x);vm.color2.d3dColor=signed_bits((u32(vm.color1.a)<<24)|0xff6080);vm.flag17=true;enemy.hit_flash=1;}
        else vm.flag17=false;
    }else{vm.color2.d3dColor=signed_bits((u32(vm.color1.a/2)<<24)|0x2020c0);vm.flag17=true;}
}
}
