#include "rewards.hpp"
#include "../gameplay/player_state.hpp"
#include "../damage_regions/damage.hpp"
#include "../bomb_system/bomb.hpp"
#include <algorithm>
#include <cstring>
#include <emmintrin.h>
namespace th20::source::item {
namespace {
namespace ps=gameplay::player_state;
using game_session::Player;
int get(const Player& p,unsigned offset){return ps::read<int>(p,offset);}
void set(Player& p,unsigned offset,int value){ps::write(p,offset,value);}
int add(int a,int b){return recovered::signed_bits(static_cast<unsigned>(a)+static_cast<unsigned>(b));}
int sub(int a,int b){return recovered::signed_bits(static_cast<unsigned>(a)-static_cast<unsigned>(b));}
int mul(int a,int b){return recovered::signed_bits(static_cast<unsigned>(a)*static_cast<unsigned>(b));}
int clamp(Player& p,unsigned offset,int lower,int upper){const auto value=std::clamp(get(p,offset),lower,upper);set(p,offset,value);return value;}
template<class T>T read(const void* object,unsigned offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(object)+offset,sizeof(value));return value;}
int truncate(float value){return _mm_cvtt_ss2si(_mm_set_ss(value));}
float subtract(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
Player& current(){return *game_session::context(0).current_player;}
void* entity(){return game_session::context(0).objects_04[0];}
void* score_owner(){return game_session::context(0).objects_04[6];}
int point_value(Player& p){return clamp(p,0x40,10000,1000000);} //4c4b30
int power(Player& p){return clamp(p,0x30,0,400);} //4993b0
int maximum_power(Player& p){return clamp(p,0x34,400,400);} //4b8210
int round_points(int amount){const auto result=sub(amount,amount%10);return result<1?10:result;}
void global_score(int amount){damage::add_score(game_session::player(0),static_cast<unsigned>(amount));} //488550 zero extends
void update_bomb_hud(Player& p,RewardEnvironment& host){if(host.hud_available()){const auto maximum=clamp(p,0xd8,0,7),fragments=clamp(p,0xd0,0,10),count=clamp(p,0xcc,0,10);host.hud_bombs(count,fragments,maximum);}}
void update_life_hud(Player& p,RewardEnvironment& host){if(host.hud_available()){const auto maximum=clamp(p,0xbc,0,7),fragments=clamp(p,0xc0,0,10),count=clamp(p,0xb8,-1,7);host.hud_lives(count,fragments,maximum);}}
}
bool add_power(Player& p,int amount,RewardEnvironment& host){
    if(get(p,0x30)>=get(p,0x34))return false;
    set(p,0x30,add(get(p,0x30),amount));
    if(get(p,0x34)<get(p,0x30)){set(p,0x30,get(p,0x34));host.hud_notice(2,0);}
    return sub(get(p,0x30),amount)/get(p,0x38)!=get(p,0x30)/get(p,0x38);
}
void add_bombs(Player& p,int amount,RewardEnvironment& host){
    set(p,0xcc,add(get(p,0xcc),amount));
    if(clamp(p,0xd8,0,7)<=get(p,0xcc))set(p,0xd0,0);
    if(clamp(p,0xd8,0,7)<get(p,0xcc))set(p,0xcc,clamp(p,0xd8,0,7));else host.sound(46);
    update_bomb_hud(p,host);
}
void add_bomb_fragments(Player& p,int amount,RewardEnvironment& host){
    if(get(p,0xcc)<clamp(p,0xd8,0,7)){
        set(p,0xd0,add(get(p,0xd0),amount));
        if(get(p,0xd0)>2){set(p,0xd0,0);add_bombs(p,1,host);}
        update_bomb_hud(p,host);
    }else set(p,0xd0,0);
}
int add_lives(Player& p,int amount,RewardEnvironment& host){
    if(clamp(p,0xbc,0,7)<=get(p,0xb8))set(p,0xc0,0);
    set(p,0xb8,add(get(p,0xb8),amount));
    if(clamp(p,0xbc,0,7)<get(p,0xb8)){set(p,0xb8,clamp(p,0xbc,0,7));set(p,0xc0,0);}
    update_life_hud(p,host);return 1;
}
void extend_life(Player& p,RewardEnvironment& host){
    if(add_lives(p,1,host)){host.sound(17);if(host.hud_available())host.hud_notice(4,0);}
    set(p,0xc4,add(get(p,0xc4),1));
}
void add_life_fragments(Player& p,int amount,int difficulty,RewardEnvironment& host){
    // The original tables contain 31 normal entries and 10 Extra entries.
    // Their index domain is inherited from the original player progression.
    static constexpr int normal[]{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,99999999};
    static constexpr int extra[]{3,3,3,3,3,3,3,3,3,99999999};
    if(get(p,0xb8)<clamp(p,0xbc,0,7)){
        set(p,0xc0,add(get(p,0xc0),amount));const auto threshold=(difficulty==4?extra:normal)[get(p,0xc4)];
        // threshold is selected once; extend_life changes c4 but not this value.
        while(threshold<=get(p,0xc0)){set(p,0xc0,sub(get(p,0xc0),threshold));extend_life(p,host);}
        update_life_hud(p,host);
    }else set(p,0xc0,0);
}
void add_point_items(Player& p,int amount) noexcept{set(p,0x3c,add(get(p,0x3c),amount));clamp(p,0x3c,0,1000000);}
void add_special_items(Player& p,int amount) noexcept{set(p,0x44,add(get(p,0x44),amount));clamp(p,0x44,0,1000000);}
void add_special_counter(Player& p,unsigned index,int amount) noexcept{
    const auto offset=0x64+index*4;set(p,offset,add(get(p,offset),amount));
    if(get(p,offset)>=1000){set(p,offset,1000);if(get(p,0x98)<0)set(p,0x98,static_cast<int>(index));}
}
void add_overlay_meter(runtime::CallbackOwner& overlay,int amount,RewardEnvironment& host){
    if(host.boss_collecting()||read<int>(&overlay,0x54)!=0)return;
    auto& p=current();const auto interval=p.bytes_2c[3]?15:10;
    if(p.bytes_a4[0]&&clamp(p,0x4c,0,500)%interval==interval-1){amount=add(amount,1);read<std::uint8_t*>(&overlay,0x34)[0x35]=1;}
    const auto count=clamp(p,0x4c,0,500),maximum=clamp(p,0x50,100,500);
    if(count<maximum){set(p,0x4c,add(get(p,0x4c),amount));clamp(p,0x4c,0,500);
        const auto after=clamp(p,0x4c,0,500),limit=clamp(p,0x50,100,500);
        if(limit<=after){set(p,0x4c,clamp(p,0x50,100,500));clamp(p,0x4c,0,500);host.start_special_phase();}
    }else if(read<int>(&overlay,0x54)==0){set(p,0x4c,clamp(p,0x50,100,500));clamp(p,0x4c,0,500);host.start_special_phase();}
}
void collect_point(Item& item,RewardEnvironment& host){
    auto& p=current();const auto base=point_value(p),special=clamp(p,0x44,0,1000000),base2=point_value(p),ratio=clamp(p,0x48,5000,10000);
    const auto value=add(base/2,mul(special,base2)/ratio);int amount;
    if(item.position.y<=read<float>(entity(),0x2090)||item.state==3){amount=round_points(value);host.floating_score(score_owner(),item.position,amount,0xffffff00);}
    else {const auto reduced=mul(value,9)/10;amount=round_points(sub(reduced,mul(reduced,truncate(subtract(item.position.y,read<float>(entity(),0x2090))))/450));host.floating_score(score_owner(),item.position,amount,0xffffffff);}
    damage::add_score(p,static_cast<std::uint64_t>(static_cast<std::int64_t>(amount)));add_point_items(p,1);
}
void collect_small_power(Item& item,RewardEnvironment& host){
    const auto player_y=read<sprite::Vec3>(entity(),0x614).y;auto& p=current();const auto current_power=power(p),maximum=maximum_power(p);int amount;
    if(current_power<maximum){amount=100;if(add_power(p,1,host)){host.refresh_power(entity(),-1);host.floating_score(score_owner(),item.position,-1,0xffffff40);host.sound_at(13,item.position.x);}}
    else {
        if(player_y<=read<float>(entity(),0x2090)||item.state==3)amount=round_points(point_value(p));
        else {const auto first=mul(point_value(p),9)/10,second=mul(point_value(p),9)/10;amount=round_points(sub(first,mul(second,sub(truncate(player_y),truncate(read<float>(entity(),0x2090))))/450));}
        host.floating_score(score_owner(),item.position,amount,0xffffffff);
    }
    global_score(amount);
}
void collect_large_power(Item& item,RewardEnvironment& host){
    auto& p=*item.context->current_player;const auto value=power(p),maximum=maximum_power(p);int amount;
    if(value<maximum){amount=100;if(add_power(p,ps::starting_power(p),host)){host.refresh_power(item.context->objects_04[0],-1);host.sound_at(13,item.position.x);host.floating_score(item.context->objects_04[6],item.position,-1,0xffffff40);}}
    else {amount=20000;global_score(20000);host.floating_score(item.context->objects_04[6],item.position,20000,0xff808080);host.sound_at(13,item.position.x);}
    global_score(amount);
}
void collect_full_power(Item& item,RewardEnvironment& host){
    auto& p=current();const auto value=power(p),maximum=maximum_power(p);
    if(maximum<=value){const auto amount=round_points(point_value(p));host.floating_score(score_owner(),item.position,amount,0xff40ff40);global_score(amount);}
    if(add_power(p,maximum_power(p),host)){host.refresh_power(entity(),-1);host.floating_score(score_owner(),item.position,-1,0xffffff40);host.sound_at(13,item.position.x);}
}
void collect_item(Item& item,RewardEnvironment& host){
    auto& p=current();switch(item.type){
    case 1:collect_small_power(item,host);break;
    case 2:case 15:collect_point(item,host);break;
    case 3:collect_large_power(item,host);break;
    case 4:add_life_fragments(p,1,gameplay::player_state::difficulty(game_session::session.player_table),host);break;
    case 5:extend_life(p,host);break;
    case 6:add_bomb_fragments(p,1,host);break;
    case 7:add_bombs(p,1,host);break;
    case 8:collect_full_power(item,host);break;
    case 9:case 10:case 11:case 12:case 13:
        add_overlay_meter(*game_session::overlay_owner(0),1,host);add_special_items(p,1);
        if(!host.special_active()){bomb::add_player_meter(p,1);add_special_counter(p,item.type==13?0:static_cast<unsigned>(item.type-9),10);}break;
    }
}
void collect_item(Item& item){collect_item(item,reward_environment());}
}
