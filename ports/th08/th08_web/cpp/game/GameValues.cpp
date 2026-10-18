#include "GameValues.hpp"
namespace th08 {
i32 GameValues::random_integer(){return i32(random.bounded32(100000)+6543);}
float GameValues::random_float(){return (random.range(100000)+number(6543)).to_float();}
i32 GameValues::checksum_bytes(const u8* data,i32 size){
    u32 sum=0;for(i32 i=0;i<size;++i){sum+=data[i];globals.integrity_value+=u32(globals.rng8[2]);}return signed_bits(sum);
}
i32 GameValues::checksum(){
    u32 sum=u32(checksum_bytes(reinterpret_cast<const u8*>(globals.rng1),0x80));
    sum+=u32(checksum_bytes(reinterpret_cast<const u8*>(globals.rng8),20));
    sum+=u32(checksum_bytes(reinterpret_cast<const u8*>(&game_config),sizeof(game_config)));
    sum+=u32(checksum_bytes(reinterpret_cast<const u8*>(&display_config),sizeof(display_config)));
    sum+=u32(checksum_bytes(reinterpret_cast<const u8*>(&high_score),sizeof(high_score)));return signed_bits(sum);
}
void GameValues::store_checksum(bool separate_casts){
    globals.integrity_value=u32(globals.rng1[2]);globals.integrity_checksum=checksum();
    if(separate_casts)expected_checksum=(number(Extended::from_int(globals.integrity_checksum).to_float())+number(Extended::from_int(globals.rng7[3]).to_float())).to_float();
    else expected_checksum=Extended::from_int(wrapping_add(globals.integrity_checksum,globals.rng7[3])).to_float();
}
void GameValues::initialize_integrity(){
    globals.rng6=random_integer();for(auto& x:globals.rng1)x=random_integer();for(auto& x:globals.rng7)x=random_integer();
    for(auto& x:globals.rng2)x=random_float();for(auto& x:globals.rng3)x=random_float();for(auto& x:globals.rng4)x=random_float();for(auto& x:globals.rng5)x=random_float();
    for(auto& x:globals.rng8)x=random_integer();store_checksum(true);
}
void GameValues::randomize_integrity(){for(u32 i=0;i<5;++i)globals.rng1[i]=random_integer();for(auto& x:globals.rng4)x=random_float();}
void GameValues::update_integrity(){globals.rng1[2]=random_integer();globals.rng7[3]=random_integer();store_checksum();}
bool GameValues::tampered()const{
    return globals.integrity_value!=u32(globals.rng1[2])+u32(globals.rng8[2])*628u ||
        wrapping_add(globals.integrity_checksum,globals.rng7[3])!=Scalar::truncate(expected_checksum);
}
void GameValues::set_lives(i32 value){globals.lives=Extended::from_int(value).to_float();update_integrity();}
void GameValues::set_bombs(i32 value){globals.bombs=Extended::from_int(value).to_float();store_checksum();}
void GameValues::set_power(i32 value){globals.power=Extended::from_int(value).to_float();update_integrity();}
void GameValues::set_deaths_stage(i32 value){globals.deaths_stage=Extended::from_int(value).to_float();update_integrity();}
void GameValues::set_bombs_stage(i32 value){globals.bombs_used_stage=Extended::from_int(value).to_float();update_integrity();}
bool GameValues::add_lives(i32 value){if(tampered())return false;globals.lives=(number(globals.lives)+Extended::from_int(value)).to_float();update_integrity();return true;}
bool GameValues::add_bombs(i32 value){if(tampered())return false;globals.bombs=(number(globals.bombs)+Extended::from_int(value)).to_float();update_integrity();return true;}
bool GameValues::add_power(i32 value){if(tampered())return false;globals.power=(number(globals.power)+Extended::from_int(value)).to_float();update_integrity();return true;}
bool GameValues::add_deaths(i32 value){if(tampered())return false;globals.deaths=(Extended::from_int(value)+number(globals.deaths)).to_float();globals.deaths_stage=(Extended::from_int(value)+number(globals.deaths_stage)).to_float();high_score.deaths=wrapping_add(high_score.deaths,1);update_integrity();return true;}
bool GameValues::count_bombs(i32 value){if(tampered())return false;globals.bombs_used=(Extended::from_int(value)+number(globals.bombs_used)).to_float();globals.bombs_used_stage=(Extended::from_int(value)+number(globals.bombs_used_stage)).to_float();update_integrity();return true;}
void GameValues::add_time_orbs(i32 value){
    if(value<0&&globals.time_orbs<wrapping_sub(0,value)){globals.time_orbs=0;return;}
    globals.time_orbs=wrapping_add(globals.time_orbs,value);globals.total_time_orbs=wrapping_add(globals.total_time_orbs,value);high_score.time_orbs=wrapping_add(high_score.time_orbs,value);update_integrity();
    if(value>0){const i32 half=wrapping_add(value,globals.total_time_orbs&1)/2;globals.point_value=wrapping_add(globals.point_value,signed_bits(u32(half)*10));}
}
const SpellMusic& spell_music(i32 current)noexcept{
    static constexpr SpellMusic entries[]{
        {1,1,"th08_00.mid",0,false},{12,2,"th08_03.mid",1,false},{16,3,"th08_04.mid",0,false},
        {31,4,"th08_05.mid",1,false},{35,5,"th08_06.mid",0,false},{53,6,"th08_07.mid",1,false},
        {76,8,"th08_09.mid",1,false},{99,9,"th08_10.mid",1,false},{118,11,"th08_12.mid",1,false},
        {122,12,"th08_13.mid",0,false},{142,13,"th08_14.mid",1,false},{146,15,"th08_13b.mid",2,true},
        {150,12,"th08_13.mid",0,false},{170,14,"th08_15.mid",1,false},{190,15,"th08_13b.mid",2,true},
        {193,16,"th08_18.mid",0,false},{204,17,"th08_19.mid",1,false},{222,20,"th08_20.mid",2,false},
        {-1,0," ",0,false}
    };
    for(const auto& entry:entries)if(entry.last_spell<0||current<=entry.last_spell)return entry;
    return entries[18];
}
}
