#pragma once
#include <cstdint>
#include <array>
namespace soew {
// th02/main/score.cpp. Signed score_t arithmetic is explicitly modulo32.
// HUD/SE are outputs; extend is tested only when score update transfers points,
// not when death commits the whole pending delta.
struct ScoreCore {
    std::uint32_t score=0,delta=0,hiscore=0;
    unsigned transferred=0,extends=0,sound8=0;
    static int signed32(std::uint32_t value){return value<0x80000000u?int(value):int(std::int64_t(value)-0x100000000ll);}
    void add(std::uint32_t amount){delta+=amount;}
    void add_hit_lowword(std::uint16_t amount){delta=(delta&0xffff0000u)|static_cast<std::uint16_t>(delta+amount);}
    void commit(){score+=delta;delta=0;transferred=0;}
    void stage_extend_init(){constexpr std::array<std::uint32_t,6> thresholds={100000,200000,300000,500000,800000,99999999};delta=0;transferred=0;extends=0;while(extends<thresholds.size()&&thresholds[extends]<=score)++extends;}
    // dialog_pre calls the bonus grant: retain delta/transferred, process one
    // extend, but do NOT turn a capped-life extend into a Bomb.
    void grant_dialogue_bonus(unsigned &lives){
        constexpr std::array<std::uint32_t,6> thresholds={100000,200000,300000,500000,800000,99999999};
        score+=delta;if(signed32(score)>signed32(hiscore))hiscore=score;
        if(extends<thresholds.size()&&thresholds[extends]<=score){if(lives<5){++lives;++sound8;}++extends;}
    }
    void update(unsigned &lives,unsigned &bombs,bool reduced=false){
        constexpr std::array<std::uint32_t,6> thresholds={100000,200000,300000,500000,800000,99999999};
        if(!delta){transferred=0;return;}
        if(reduced)transferred=3111;
        else {const int signed_delta=signed32(delta);const auto part=static_cast<unsigned>(signed_delta<32?1:signed_delta/32);if(transferred<part)transferred=part>1111?1111:part;}
        const auto remain=delta-transferred;if(signed32(remain)<0){transferred=delta;delta=0;}else delta=remain;
        score+=transferred;if(signed32(score)>signed32(hiscore))hiscore=score;
        if(extends<thresholds.size()&&thresholds[extends]<=score){if(lives<5){++lives;++sound8;}else {if(bombs<5)++bombs;++sound8;}++extends;}
    }
};
}
