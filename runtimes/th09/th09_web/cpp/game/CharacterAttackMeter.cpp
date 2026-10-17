#include "CharacterAttackMeter.hpp"
namespace th09 {
bool CharacterAttackMeter::consume(ComboState& combo,i32 character,i32 rank,i32 side,const Vec3& position,AttackQueueActions& actions){
    if(character<0||character>=16)return false;
    static constexpr i32 attack_kind[16]={0,2,3,13,14,5,15,6,16,17,18,19,20,21,25,26};
    static constexpr i32 threshold_base[16]={140,120,90,70,140,25,100,200,100,60,100,140,160,140,100,80};
    static constexpr i32 threshold_rank[16]={5,3,2,3,5,0,3,3,3,2,4,5,4,5,3,3};
    static constexpr i32 cost_base[16]={140,120,90,70,140,25,140,200,100,60,100,140,160,140,140,80};
    static constexpr i32 cost_rank[16]={5,4,2,0,5,0,5,3,3,2,4,5,4,5,5,3};
    const auto product=[](i32 a,i32 b){return signed_bits(u32(a)*u32(b));};
    const i32 threshold=wrapping_sub(threshold_base[character],character==5?rank/2:product(rank,threshold_rank[character]));
    const i32 cost=wrapping_sub(cost_base[character],character==5?rank/2:product(rank,cost_rank[character]));
    if(cost<=0)return false;
    while(combo.character_attack>=threshold){
        actions.queue_attack(attack_kind[character],side,position,nullptr);combo.character_attack=wrapping_sub(combo.character_attack,cost);
        if(character==7){combo.spirit_attack=wrapping_sub(combo.spirit_attack,5);if(combo.spirit_attack<0)combo.spirit_attack=0;}
    }
    return true;
}
}
