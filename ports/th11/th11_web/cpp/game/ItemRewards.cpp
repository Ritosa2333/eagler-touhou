#include "ItemRewards.hpp"
#include "ItemManager.hpp"
namespace th11 {
bool ItemRewards::add_power(i32 amount,bool& changed){
    changed=false;if(state.power>=state.max_power)return true;
    if(state.power_step<=0)return false;
    state.power=wrapping_add(state.power,amount);
    if(state.power>state.max_power){state.power=state.max_power;if(!effects.notify(2))return false;}
    // The original subtracts the award from the clamped value here.
    changed=wrapping_sub(state.power,amount)/state.power_step!=state.power/state.power_step;return true;
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
bool ItemRewards::collect(const ItemState& item,bool& convert){
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
