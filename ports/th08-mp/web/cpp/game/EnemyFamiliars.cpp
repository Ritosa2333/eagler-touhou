#include "EnemyFamiliars.hpp"
#include "GameMath.hpp"
namespace th08 {
void unlink_familiar(EclVm& enemy)noexcept{
    if(enemy.parent){
        if(!enemy.previous_familiar){enemy.invalid=true;return;}
        enemy.previous_familiar->next_familiar=enemy.next_familiar;
        if(enemy.next_familiar)enemy.next_familiar->previous_familiar=enemy.previous_familiar;
        enemy.parent=nullptr;
    }
    enemy.previous_familiar=enemy.next_familiar=nullptr;
}
void EnemyFamiliars::scatter(const Vec3& origin,float limit,i32 mode){
    const float radius=random.range(limit).to_float(),angle=random.signed_range(3.1415927410125732f).to_float();
    Vec3 p{(cosine(angle)*number(radius)).to_float(),(sine(angle)*number(radius)).to_float(),0};
    p={Scalar::add(p.x,origin.x),Scalar::add(p.y,origin.y),Scalar::add(p.z,origin.z)};actions.item(p,7,mode);
}
bool EnemyFamiliars::clear(EclVm& enemy,bool reward){
    i32 count=0;if(!enemy.parent)for(auto* p=enemy.next_familiar;p;p=p->next_familiar){if(++count>480){enemy.invalid=true;return false;}}
    if(count){
        const u32 color=count<2?0xffffffff:count<6?0xffffffd0:count<10?0xffffffb0:0xffffff80;i32 index=0;
        for(auto* child=enemy.next_familiar;child;){
            if(child->flags&0x200)child->position_offset=enemy.position;
            auto* next=child->next_familiar;child->flags|=0x400;child->parent=child->next_familiar=child->previous_familiar=nullptr;
            if(reward){
                const i32 cancel_item=enemy.flags&2?7:9;
                const i32 items=input.character>=4?(input.character&1?(count<10?count*2+6:26):(count<4?count*6+16:40)):(count<8?count*2+10:26);
                if(input.bomb)count/=3;
                actions.popup(child->resolved_position,count,0,color);child->refresh_position();
                regions.circle(false,{child->resolved_position.x,child->resolved_position.y},32,2,cancel_item,8);
                for(i32 i=0;i<items;++i)scatter(child->resolved_position,(Extended::from_int(items)+Extended::from_int(items)).to_float(),3);
                if(!input.boss_present||input.time_spell){child->item_reward=8;drop_enemy_items(*child,false,drops,random,actions);if(child->invalid)return false;}
                actions.panned_sound(index%2+2,child->resolved_position.x);
            }
            child->power_items=child->point_items=0;child->item_reward=-2;++index;child=next;
        }
        if(reward){
            actions.popup_scale(2,2);actions.popup(enemy.resolved_position,enemy.summoned_familiars,0,0xfff0f00f);actions.popup_scale(1,1);
            for(i32 i=0;i<wrapping_add(enemy.summoned_familiars,enemy.summoned_familiars);++i)scatter(enemy.resolved_position,128,1);
            regions.circle(false,{enemy.resolved_position.x,enemy.resolved_position.y},32,1,7,16);input.item_gauge_lock.set(0);
        }
    }
    if(enemy.parent&&reward){
        gauge.add(i16(-i32(gauge.value())/12),input.bomb,false);input.form_transition.set(0);input.gauge_idle.set(30);input.item_gauge_lock.set(50);
        enemy.refresh_position();actions.popup(enemy.resolved_position,1,0,0xffffffff);actions.item(enemy.resolved_position,7,1);
        enemy.power_items=enemy.point_items=0;enemy.item_reward=-2;
    }
    unlink_familiar(enemy);return !enemy.invalid;
}
}
