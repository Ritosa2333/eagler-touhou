#include "SessionSetup.hpp"
namespace th08::multiplayer {
bool decode_session_setup(SessionSetup& current,const std::uint32_t* words,std::size_t size) noexcept {
    if(current.started||!words||size!=11||words[0]!=1||words[1]<2||words[1]>3||
       words[2]>=words[1]||words[3]>4||words[4]>65535)return false;
    SessionSetup next{};
    next.player_count=words[1];next.local_player=words[2];
    next.difficulty=words[3];next.seed=words[4];
    for(std::uint32_t seat=0;seat<3;++seat){
        const auto character=words[5+seat*2],shot=words[6+seat*2];
        if(character>11||shot||(seat>=next.player_count&&character))return false;
        next.characters[seat]=character;
    }
    next.configured=true;current=next;return true;
}
std::uint32_t gameplay_contract(const SessionSetup& setup) noexcept {
    std::uint32_t hash=2166136261u;
    const auto word=[&](std::uint32_t value){for(int i=0;i<4;++i){hash^=(value>>(i*8))&255u;hash*=16777619u;}};
    word(0x08000001u);word(setup.player_count);word(setup.difficulty);word(setup.seed);
    for(const auto character:setup.characters)word(character);
    return hash;
}
}
