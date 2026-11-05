#include "EnemyDeath.hpp"
#include <algorithm>
namespace th09 {
bool EnemyDeath::apply(EclVm& enemy,i32 source,const PlayfieldGeometry& geometry,float opposing_width,EnemyDeathActions& actions){
    auto& v=enemy.values;if(!v.world||!v.field)return false;auto& random=v.world->random;
    if(source)actions.reset_chain();
    if(v.flags&0x2000){actions.play_positioned_sound(18,v.resolved_position.x);actions.chain_kill(v.resolved_position,400,0,300,1000);actions.drop_item(v.item_reward,v.position);return true;}
    if(v.flags&0xc00){actions.play_positioned_sound(18,v.resolved_position.x);actions.drop_item(random.below(4),v.position);}
    actions.play_positioned_sound(enemy.status.index%2+2,v.resolved_position.x);
    const i32 effect=enemy.status.effects[0];
    if(!(v.flags&0x1c0)||source==0||(v.flags&0x1000)){
        actions.effect(effect+20,v.resolved_position);const float speed=effect==0?5.f:effect==1?5.5f:effect==2?6.f:6.25f;
        actions.explosion(v.resolved_position,32,speed,(v.flags&0x1c0)?2:3,effect+8,4);
    }else actions.effect(5,v.resolved_position);
    const u32 kind=(v.flags>>6)&7;const i32 special=kind==0?4:kind==1?10:kind==2?20:kind==3?30:50;
    const i32 count=actions.chain_count();const i32 normal=count<11?wrapping_add(signed_bits(u32(count)*3),30):60;
    const i32 spirits=kind==0?(count<25?wrapping_add(count,4):28):0;
    i32 score=kind==0?(count<101?wrapping_add(signed_bits(u32(count)*30),10):3000):(count<51?signed_bits(u32(wrapping_add(count,5))*200):11000);
    if(source)score=signed_bits(u32(score)*2);
    actions.chain_kill(v.resolved_position,normal,spirits,special,score);
    const i32 after=actions.chain_count();actions.charge(after<129?float(after)*.0078125f+1.f:2.f);
    if(kind>0&&kind<3&&source<2&&actions.opposing_spirits()<25){
        const Vec3 screen=geometry.to_screen(v.position);
        const float x=float(random.signed_unit())*(opposing_width*.5f-8.f),y=float(random.range(128));
        auto transfer=actions.create_transfer(v.field->side+3,screen,{x,y,0});if(!transfer)return false;
        transfer->source_kind=i16(std::min(kind,2u));transfer->target_kind=i16(std::min(kind+1,3u));
        transfer->speed=float(random.range(.5f))*.016666668f;transfer->source_side=i16(v.field->side);
    }
    return true;
}
}
