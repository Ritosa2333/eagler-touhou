#include "ItemRewards.hpp"
namespace th08 {
namespace {
i32 integer(float value){return Scalar::truncate(value);}
i32 times(i32 a,i32 b){return signed_bits(u32(a)*u32(b));}
i32 power_level(i32 power){static constexpr i32 thresholds[]{8,24,48,80,128,999};i32 level=0;while(level<5&&power>=thresholds[level])++level;return level;}
}
void ItemRewards::power(ItemState& item,bool big){
    if(integer(globals.power)<128){
        const i32 level=power_level(integer(globals.power));if(!big)context.power_flag=0;
        if(!values.add_power(big?8:1)){failed=true;return;}
        if(integer(globals.power)>=128){values.set_power(128);if(!context.time_spell)actions.cancel_bullets();actions.gui_popup(0,1);pool.convert_power(&item);}
        if(big)context.hud_flags=(context.hud_flags&~0x30u)|0x20;values.add_score(10);if(!big)context.hud_flags=(context.hud_flags&~0x30u)|0x20;
        if(power_level(integer(globals.power))==level)actions.popup(item.position,10,0xffffffff,false);
        else{actions.popup(item.position,-1,0xffffc0a0,false);actions.sound(31,0);}
    }if(!big)rank.add(1);
}
void ItemRewards::extend(){
    if(integer(globals.lives)<8){if(!values.add_lives(1)){failed=true;return;}actions.sound(28,0);rank.add(200);context.hud_flags=(context.hud_flags&~3u)|2;}
    else if(integer(globals.bombs)<8){if(!values.add_bombs(1)){failed=true;return;}actions.sound(28,0);rank.add(200);context.hud_flags=(context.hud_flags&~12u)|8;}
}
void ItemRewards::point(ItemState& item,bool small){
    i32 maximum=globals.point_value,value=maximum;
    if(!(item.position.y<context.collect_line))value=wrapping_sub(maximum/2,times((number(item.position.y)-number(context.collect_line)).truncate_int(),globals.point_value/1000));
    if(item.max_value==1)value=maximum;
    if(small){maximum=maximum/10-maximum/10%10;value=value/10-value/10%10;}else value-=value%10;
    if(gauge.human_bonus())value=wrapping_add(value,value);
    actions.popup(item.position,value,value<maximum?0xffffffff:0xffffff00,false);
    if(!small&&value>=maximum)item.max_value=1;values.add_score(value);if(small){if(value>=maximum)item.max_value=1;return;}
    globals.points_stage=wrapping_add(globals.points_stage,1);globals.points=wrapping_add(globals.points,1);context.hud_flags=(context.hud_flags&~0x300u)|0x200;rank.add(value<maximum?3:10);
    if(signed_bits(globals.point_extends)>=0){
        for(;;){point_item_extend_threshold(globals,context.difficulty);if(globals.points<globals.next_point_extend)break;extend();++globals.point_extends;}
    }
    high_score.points=wrapping_add(high_score.points,1);values.update_integrity();
}
void ItemRewards::time_orb(ItemState* item){
    i32 value=100;if(!context.bomb_triggered){if(globals.points_stage<2000){value=times(globals.points/2,10);if(value<100)value=100;}else value=10000;}
    if(item)actions.popup(item->position,value,globals.time_orbs>=globals.last_spell_requirement?0xdfffef80:0xdfffffff,true);
    context.hud_flags=(context.hud_flags&~0xc00u)|0x800;values.add_score(value);values.add_time_orbs(1);actions.spell_time(8000);
    if(context.gauge_lock.current==0)gauge.add(context.focused?111:-111,context.bomb_active,false);
}
void ItemRewards::collect(ItemState& item){
    switch(item.type){
    case 0:power(item,false);break;case 1:point(item,false);break;case 2:power(item,true);break;
    case 3:if(integer(globals.bombs)<8){failed|=!values.add_bombs(1);context.hud_flags=(context.hud_flags&~12u)|8;}rank.add(5);break;
    case 4:
        if(integer(globals.power)<128){actions.cancel_bullets();actions.gui_popup(0,1);actions.sound(31,0);actions.popup(item.position,-1,0xffffc0a0,false);pool.convert_power(&item);}
        values.set_power(128);values.add_score(1000);actions.popup(item.position,1000,0xffffffff,false);context.hud_flags=(context.hud_flags&~0x30u)|0x20;break;
    case 5:extend();break;
    case 6:{i32 value=context.bomb_triggered?100:wrapping_add(times(globals.graze/40,10),300);if(!context.bomb_triggered&&value<1)value=10;actions.popup(item.position,value,0xffffffff,true);values.add_score(value);break;}
    case 7:time_orb(&item);break;case 8:point(item,true);break;default:break;
    }
}
}
