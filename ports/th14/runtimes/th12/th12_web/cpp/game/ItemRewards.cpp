#include "ItemRewards.hpp"
#include "ItemManager.hpp"
#include "Th14LifeFragments.hpp"
#include "PowerAddWordsTh14.hpp"
#include "HudScalarTh14.hpp"
#include <algorithm>
namespace th11 {
#if PORT_TITLE == 16
bool ItemRewards::th16_add_score(i32 amount){
    // Complete43e080/43ddd0 on the finite retail extend tables.
    constexpr u32 normal[]={500000,1000000,2000000,4000000,7000000,10000000,15000000,25000000,50000000,100000000,1000000000};
    constexpr u32 extra[]={1000000,2000000,4000000,6000000,8000000,10000000,1000000000};
    const auto* table=state.difficulty==4?extra:normal;const u32 size=state.difficulty==4?7:11;
    state.score_units=wrapping_add(state.score_units,amount/10);
    for(;;){
        if(u32(state.th16_extend_index)>=size)return false;
        if(u32(state.score_units)<table[state.th16_extend_index])break;
        if(state.lives<8){
            state.lives=std::min(wrapping_add(state.lives,1),8);
            if(!effects.lives_changed(state.lives,state.life_fragments)||!effects.sound(17,0,false)||!effects.notify(4))return false;
        }
        state.th16_extend_index=wrapping_add(state.th16_extend_index,1);
    }
    if(u32(state.score_units)>=1000000000)state.score_units=999999999;
    return true;
}
bool ItemRewards::th16_life(bool& awarded){
    awarded=state.lives<8;if(!awarded)return true;
    state.lives=std::min(wrapping_add(state.lives,1),8);
    return effects.lives_changed(state.lives,state.life_fragments);
}
bool ItemRewards::th16_bomb(){
    state.th16_bombs=wrapping_add(state.th16_bombs,1);
    if(state.th16_bombs>8)state.th16_bombs=8;
    else if(!effects.sound(46,0,false))return false;
    return effects.bombs_changed(state.th16_bombs,state.th16_bomb_fragments);
}
bool ItemRewards::th16_bomb_fragment(){
    if(state.th16_bombs>=8){state.th16_bomb_fragments=0;return true;}
    state.th16_bomb_fragments=wrapping_add(state.th16_bomb_fragments,1);
    if(state.th16_bomb_fragments>=5){
        state.th16_bombs=wrapping_add(state.th16_bombs,1);state.th16_bomb_fragments=0;
        if(state.th16_bombs>8)state.th16_bombs=8;
        else if(!effects.sound(46,0,false))return false;
        if(!effects.bombs_changed(state.th16_bombs,state.th16_bomb_fragments))return false;
    }
    return effects.bombs_changed(state.th16_bombs,state.th16_bomb_fragments);
}
bool ItemRewards::th16_collect(const ItemState& item,const ItemPlayer& player){
    if(item.type==4)return true; // Own4300bc maps type4 directly to release.
    if(item.type==5){bool award=false;if(!th16_life(award))return false;return !award||(effects.sound(17,0,false)&&effects.notify(4));}
    if(item.type==6)return th16_bomb_fragment();
    if(item.type==7)return th16_bomb();
    const i32 line=player.character==3?148:128;
    const bool auto_collect=player.position.y<=float(line)||item.state==4;
    const auto record=[&](i32 award){state.th16_auto_total=wrapping_add(state.th16_auto_total,award);state.th16_auto_count=wrapping_add(state.th16_auto_count,1);state.th16_auto_position=player.position;};
    const auto point_score=[&](){
        const i32 base=state.point_value/100,rounded=wrapping_sub(base,base%10);
        if(auto_collect)return std::max(10,signed_bits(u32(rounded/10)*10));
        const i32 slope=signed_bits(u32(wrapping_sub(0,rounded))*3)/4;
        const i32 distance=wrapping_sub(truncate_int(player.position.y),line);
        const i32 value=wrapping_add(signed_bits(u32(slope)*u32(distance))/450,signed_bits(u32(rounded)*3)/4);
        return std::max(10,signed_bits(u32(value/10)*10));
    };
    if(item.type==2){
        const auto award=point_score();if(!effects.popup(item.position,award,auto_collect?0xffffff00:0xffffffff))return false;
        if(auto_collect)record(award);
        if(!th16_add_score(award))return false;state.th16_point_items=wrapping_add(state.th16_point_items,1);return true;
    }
    if(item.type==8){
        if(state.power>=state.max_power){
            state.add_point_value(10000);
            if(!effects.popup(item.position,100,0xff40ff40)||!effects.sound(13,item.position.x,true))return false;
        }
        bool changed=false;if(!add_power(state.max_power,changed))return false;
        return !changed||(effects.power_changed()&&effects.popup(item.position,-1,0xffffff40)&&effects.sound(13,item.position.x,true));
    }
    if(item.type!=1&&item.type!=3)return false;
    i32 award=100;
    if(state.power>=state.max_power){
        if(item.type==3){award=20000;if(!th16_add_score(award)||!effects.popup(item.position,award,0xff808080)||!effects.sound(13,item.position.x,true))return false;}
        else{
            award=point_score();
            if(!effects.popup(item.position,award,auto_collect?0xffffff00:0xffffffff))return false;
            if(auto_collect)record(award);
            if(!effects.popup(item.position,award,0xffffffff))return false;
        }
    }else{
        bool changed=false;if(!add_power(item.type==1?1:state.power_step,changed))return false;
        if(changed){
            if(!effects.power_changed())return false;
            if(item.type==1){if(!effects.popup(item.position,-1,0xffffff40)||!effects.sound(13,item.position.x,true))return false;}
            else if(!effects.sound(13,item.position.x,true)||!effects.popup(item.position,-1,0xffffff40))return false;
        }
    }
    if(!th16_add_score(award))return false;if(auto_collect)record(award);return true;
}
#endif
#if PORT_TITLE == 14
bool ItemRewards::th14_bomb_fragment(){
    if(state.th14_bombs>=8){state.th14_bomb_fragments=0;return true;}
    state.th14_bomb_fragments=wrapping_add(state.th14_bomb_fragments,1);
    if(state.th14_bomb_fragments>=8){
        state.th14_bombs=wrapping_add(state.th14_bombs,1);state.th14_bomb_fragments=0;
        if(state.th14_bombs>8)state.th14_bombs=8;
        else if(!effects.sound(46,0,false))return false;
        if(!effects.bombs_changed(state.th14_bombs,state.th14_bomb_fragments))return false;
    }
    return effects.bombs_changed(state.th14_bombs,state.th14_bomb_fragments);
}
bool ItemRewards::th14_life_fragment(){
    if(state.lives>=8){if(!th14_bomb_fragment())return false;state.life_fragments=0;return true;}
    state.life_fragments=wrapping_add(state.life_fragments,1);
    while(state.life_fragments>=th14_life_fragment_requirement(state.th14_life_awards)){
        state.life_fragments=wrapping_sub(state.life_fragments,th14_life_fragment_requirement(state.th14_life_awards));
        if(state.lives<8){state.lives=std::min(wrapping_add(state.lives,1),8);
            if(!effects.lives_changed(state.lives,state.life_fragments)||!effects.sound(17,0,false)||!effects.notify(4))return false;}
        state.th14_life_awards=wrapping_add(state.th14_life_awards,1);
    }
    return effects.lives_changed(state.lives,state.life_fragments);
}
bool ItemRewards::th14_life(){
    if(state.lives>=8)return true;state.lives=std::min(wrapping_add(state.lives,1),8);
    return effects.lives_changed(state.lives,state.life_fragments);
}
bool ItemRewards::th14_bomb(){
    state.th14_bombs=wrapping_add(state.th14_bombs,1);
    if(state.th14_bombs>8)state.th14_bombs=8;
    else if(!effects.sound(46,0,false))return false;
    return effects.bombs_changed(state.th14_bombs,state.th14_bomb_fragments);
}
bool ItemRewards::th14_combo_settle(bool dialogue){
    // 438dc6..439071, after the item loop has decremented the shared timer to zero.
    auto& combo=state.th14_combo;Vec3 p=combo.position;p.y-=48.f;
    i32 amount=combo.total;u32 color=0;float multiplier=0;bool fragment=false;
    const auto round10=[](i32 n){return signed_bits(u32(n/10)*10);};
    if(combo.count>=60){amount=wrapping_add(amount,amount);color=0xffffff00;multiplier=2.f;fragment=true;}
    else if(combo.count>=50){amount=round10(wrapping_add(amount,signed_bits(u32(amount)*3)/5));color=0xe0ffff60;multiplier=1.6f;}
    else if(combo.count>=40){amount=round10(wrapping_add(amount,amount/5));color=0xd0c0ff80;multiplier=1.2f;}
    else if(combo.count>=30){
        // Signed high-product division with 99999999 is a negative fifth.
        const i64 product=i64(amount)*i64(-1717986919);const i32 high=i32(product>>33);
        amount=round10(wrapping_add(amount,wrapping_add(high,i32(u32(high)>>31))));color=0xc0a0ffa0;multiplier=.8f;
    }else if(combo.count>=20){amount=round10(amount/2);color=0xb040ffff;multiplier=.5f;}
    else if(combo.count>=5){if(!effects.th14_combo_popup(p,-1,0x80a0a0a0,0.f))return false;amount=0;}
    else amount=0;
    if(color){if(!effects.sound(57,p.x,true)||!effects.th14_combo_popup(p,amount,color,multiplier))return false;
        if(amount>0){state.add_score(amount);if(!effects.th14_combo_drop(fragment?4:12,p,dialogue?0:40))return false;}}
    combo.count=combo.total=0;return true;
}
#endif
bool ItemRewards::add_power(i32 amount,bool& changed){
#if PORT_TITLE==14
    return th14_add_power_words(amount,changed,[&](PowerAddAccessTh14 access){
        switch(access){
        case PowerAddAccessTh14::Initial:case PowerAddAccessTh14::Final:return th14_hud_load(state.power);
        case PowerAddAccessTh14::Maximum:return th14_hud_load(state.max_power);
        default:return th14_hud_load(state.power_step);
        }
    },[&](PowerAddAccessTh14,i32 value){th14_hud_store(state.power,value);},
    [&](PowerAddAccessTh14){return effects.notify(2);});
#else
    changed=false;if(state.power>=state.max_power)return true;
    if(state.power_step<=0)return false;
    state.power=wrapping_add(state.power,amount);
    if(state.power>state.max_power){state.power=state.max_power;if(!effects.notify(2))return false;}
    // The original subtracts the award from the clamped value here.
    changed=wrapping_sub(state.power,amount)/state.power_step!=state.power/state.power_step;return true;
#endif
}
bool ItemRewards::add_life(){
    state.lives=wrapping_add(state.lives,1);
    if(state.lives>9)state.lives=9;
    else if(!effects.sound(0x2c,0,false)||!effects.notify(4))return false;
    return effects.lives_changed(state.lives,i16(state.life_fragments));
}
bool ItemRewards::add_life_fragment(){
    if(state.lives>=9){state.life_fragments=0;return true;}
    state.life_fragments=wrapping_add(state.life_fragments,1);
    if(state.life_fragments>=5){state.life_fragments=0;if(!add_life())return false;}
    return effects.lives_changed(state.lives,i16(state.life_fragments));
}
bool ItemRewards::collect(const ItemState& item,bool& convert,const ItemPlayer* player){
#if PORT_TITLE == 14
    if(item.type==4)return th14_life_fragment();
    if(item.type==5)return th14_life();
    if(item.type==6)return th14_bomb_fragment();
    if(item.type==7)return th14_bomb();
    if(item.type==8){
        if(state.power>=state.max_power){
            state.add_point_value(10000);
            return effects.popup(item.position,100,0xff40ff40)&&effects.sound(13,item.position.x,true);
        }
        if(state.power_step<=0)return false;state.power=wrapping_add(state.power,state.max_power);
        if(state.power>state.max_power){state.power=state.max_power;if(!effects.notify(2))return false;}
        if(wrapping_sub(state.power,state.max_power)/state.power_step!=state.power/state.power_step)
            return effects.power_changed()&&effects.popup(item.position,-1,0xffffff40)&&effects.sound(13,item.position.x,true);
        return true;
    }
    if(item.type==1||item.type==2||item.type==3){
        if(!player)return false;
        const float line=player->character==1?148.f:128.f;
        const bool combo=player->position.y<=line||item.state==3;
        const auto record_combo=[&](i32 award){auto& c=state.th14_combo;c.count=wrapping_add(c.count,1);c.total=wrapping_add(c.total,award);c.position=player->position;};
        if(item.type==3){
            i32 award=100;
            if(state.power>=state.max_power){
                award=20000;state.add_score(award);
                if(!effects.popup(item.position,award,0xff808080)||!effects.sound(13,item.position.x,true))return false;
            }else{
                if(state.power_step<=0)return false;state.power=wrapping_add(state.power,state.power_step);
                if(state.power>state.max_power){state.power=state.max_power;if(!effects.notify(2))return false;}
                if(wrapping_sub(state.power,state.power_step)/state.power_step!=state.power/state.power_step){
                    if(!effects.power_changed()||!effects.sound(13,item.position.x,true)||!effects.popup(item.position,-1,0xffffff40))return false;
                }
            }
            state.add_score(award);if(combo)record_combo(award);return true;
        }
        if(item.type==1){
            i32 award=100;
            if(state.power>=state.max_power){award=10000;if(!effects.popup(item.position,award,0xffffffff))return false;}
            else{
                if(state.power_step<=0)return false;
                state.power=wrapping_add(state.power,1);
                if(state.power>state.max_power){state.power=state.max_power;if(!effects.notify(2))return false;}
                if(wrapping_sub(state.power,1)/state.power_step!=state.power/state.power_step){
                    if(!effects.power_changed()||!effects.popup(item.position,-1,0xffffff40)||!effects.sound(13,item.position.x,true))return false;
                }
            }
            state.add_score(award);if(combo)record_combo(award);return true;
        }
        const i32 base=state.point_value/100,rounded=wrapping_sub(base,base%10);
        const i32 distance=wrapping_sub(truncate_int(player->position.y),player->character==1?148:128);
        const i32 slope=signed_bits(u32(wrapping_sub(0,rounded))*3)/4;
        const i32 drop=signed_bits(u32(slope)*u32(distance))/450;
        const i32 value=wrapping_add(drop,signed_bits(u32(rounded)*3)/4);
        const i32 award=combo?std::max(10,signed_bits(u32(rounded/10)*10)):std::max(10,signed_bits(u32(value/10)*10));
        if(!effects.popup(item.position,award,combo?0xffffff00:0xffffffff))return false;
        if(combo)record_combo(award);
        state.add_score(award);state.th14_point_items=wrapping_add(state.th14_point_items,1);return true;
    }
    // 438b95/438b9f -> 439d30 + 41d5e0: cancel items never award power.
    if(item.type<9||item.type>11)return false;
    state.add_point_value(item.type==11?2000:200);state.add_score(10);return true;
#endif
    bool changed=false;const auto p=item.position;
    switch(item.type){
    case 1:case 10:case 6:
        if(!add_power(item.type==6?state.max_power:1,changed))return false;
        if(changed){
            if(!effects.power_changed()||!effects.popup(p,-1,0xffffff40)||!effects.sound(0x1d,p.x,true))return false;
            if(state.power>=state.max_power)convert=true;state.add_rank(12);
        }break;
    case 4:case 11:
        if(!add_power(state.power_step,changed))return false;
        if(changed){
            if(!effects.power_changed()||!effects.sound(0x1d,p.x,true)||!effects.popup(p,-1,0xffffff40))return false;
            state.add_rank(24);if(state.power>=state.max_power)convert=true;
        }else if(!effects.popup(p,state.power/2,0xffff4040))return false;
        break;
    case 2:{
        const auto value=state.point_item_score();
        if(!effects.popup(p,value,state.communication>=10000?0xffffff00:0xffffffff))return false;
        state.add_score(value);break;
    }
    case 3:{
        const i32 amount=u32(state.difficulty)<=4?10000:5000;
        if(!effects.popup(p,amount/100,0xff00ff00))return false;
        state.add_point_value(amount);break;
    }
    case 5:if(!add_life_fragment())return false;state.add_rank(256);break;
    case 7:if(!add_life())return false;state.add_rank(256);break;
    case 8:state.add_point_value(10);state.add_score(10);break;
    case 9:if(!effects.popup(p,10,0xff00ff00))return false;state.add_point_value(1000);break;
    }
    return true;
}
}
