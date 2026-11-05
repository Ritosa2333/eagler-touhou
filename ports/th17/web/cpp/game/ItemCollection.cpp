#include "ItemCollection.hpp"
namespace th17 {
void ItemCollection::add_score(i32 value)noexcept{score.score=wrapping_add(score.score,value/10);if(score.score>999999999)score.score=999999999;}
i32 ItemCollection::point_value(const ItemState& item,u32& color)const noexcept{
    const i32 threshold=character==1?148:128,p=score.point_value/100,base=wrapping_sub(p,p%10);i32 value;
    if(motion.position.y<=float(threshold)||item.state==3){color=0xffffff00;value=wrapping_mul(base/10,10);}
    else{color=0xffffffff;const i32 a=wrapping_mul(-base,3)/4,b=wrapping_mul(base,3)/4,delta=wrapping_sub(truncate_int(motion.position.y),threshold);value=wrapping_mul(wrapping_add(wrapping_mul(a,delta)/450,b)/10,10);}
    return value<1?10:value;
}
bool ItemCollection::power(const ItemState& item,bool large){
    const i32 amount=large?player.power_step:1;i32 value=100;
    if(player.power<score.max_power){player.power=wrapping_add(player.power,amount);if(player.power>score.max_power){player.power=score.max_power;if(!world.notice(2))return false;}
        if(wrapping_sub(player.power,amount)/player.power_step!=player.power/player.power_step){if(!world.options_changed())return false;if(large&&!world.sound(13,false))return false;if(!world.popup(item.position,-1,0xffffff40))return false;if(!large&&!world.sound(13,false))return false;}}
    else if(large){value=20000;add_score(value);if(!world.popup(item.position,value,0xff808080)||!world.sound(13,false))return false;}
    else{u32 color;value=point_value(item,color);if(!world.popup(item.position,value,color)||!world.popup(item.position,value,0xffffffff))return false;}
    add_score(value);return true;
}
bool ItemCollection::point(const ItemState& item){u32 color;const i32 value=point_value(item,color);if(!world.popup(item.position,value,color))return false;add_score(value);score.point_items=wrapping_add(score.point_items,1);return true;}
bool ItemCollection::full_power(const ItemState& item){
    if(player.power>=score.max_power){score.point_value=wrapping_add(score.point_value,10000);if(score.point_value>score.max_point_value)score.point_value=score.max_point_value;if(!world.popup(item.position,100,0xff40ff40)||!world.sound(13,false))return false;if(player.power>=score.max_power)return true;}
    player.power=wrapping_add(player.power,score.max_power);if(player.power>score.max_power){player.power=score.max_power;if(!world.notice(2))return false;}
    if(wrapping_sub(player.power,score.max_power)/player.power_step!=player.power/player.power_step){if(!world.options_changed()||!world.popup(item.position,-1,0xffffff40)||!world.sound(13,false))return false;}return true;
}
bool ItemCollection::life(){if(player.extra_lives>=8)return true;player.extra_lives=wrapping_add(player.extra_lives,1);if(player.extra_lives>8)player.extra_lives=8;return world.life_hud(player.extra_lives,player.life_pieces)&&world.sound(17,true)&&world.notice(4);}
bool ItemCollection::bomb(){player.bombs=wrapping_add(player.bombs,1);if(player.bombs<9){if(!world.sound(46,true))return false;}else player.bombs=8;return world.bomb_hud(player.bombs,player.bomb_pieces);}
bool ItemCollection::bomb_piece(){if(player.bombs>=8){player.bomb_pieces=0;return true;}player.bomb_pieces=wrapping_add(player.bomb_pieces,1);if(player.bomb_pieces>2){player.bombs=wrapping_add(player.bombs,1);player.bomb_pieces=0;if(player.bombs<9){if(!world.sound(46,true))return false;}else player.bombs=8;if(!world.bomb_hud(player.bombs,player.bomb_pieces))return false;}return world.bomb_hud(player.bombs,player.bomb_pieces);}
bool ItemCollection::life_piece(){
    if(player.extra_lives>=8){if(!bomb_piece())return false;player.life_pieces=0;return true;}
    if(score.life_piece_tier<0||(score.difficulty==4&&score.life_piece_tier>9)){error="Life-piece tier outside the original threshold table";return false;}
    player.life_pieces=wrapping_add(player.life_pieces,1);const i32 required=score.difficulty==4&&score.life_piece_tier==9?99999999:3;
    while(player.life_pieces>=required){player.life_pieces=wrapping_sub(player.life_pieces,required);if(player.extra_lives<8){player.extra_lives=wrapping_add(player.extra_lives,1);if(player.extra_lives>8)player.extra_lives=8;if(!world.life_hud(player.extra_lives,player.life_pieces)||!world.sound(17,true)||!world.notice(4))return false;}score.life_piece_tier=wrapping_add(score.life_piece_tier,1);}
    return world.life_hud(player.extra_lives,player.life_pieces);
}
bool ItemCollection::award(const ItemState& item){
    error.clear();if(player.power_step<=0||character<0||character>2){error="Invalid item collection configuration";return false;}bool result=true;
    switch(item.kind){case 1:result=power(item,false);break;case 2:result=point(item);break;case 3:result=power(item,true);break;case 4:result=life_piece();break;case 5:result=life();break;case 6:result=bomb_piece();break;case 7:result=bomb();break;case 8:result=full_power(item);break;
    case 9:case 10:case 11:case 12:case 13:case 14:{constexpr i32 values[]={500,1000,2000,3000,4000,5000};score.point_value=wrapping_add(score.point_value,values[item.kind-9]);if(score.point_value>score.max_point_value)score.point_value=score.max_point_value;const i32 p=score.point_value/100;score.score=wrapping_add(score.score,wrapping_sub(p,p%10)/100);if(score.score>999999999)score.score=999999999;break;}
    default:error="Invalid collected item kind";return false;}
    if(!result&&error.empty())error="Item collection world service failed";return result;
}
}
