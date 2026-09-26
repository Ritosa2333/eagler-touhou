#include "SessionSetup.hpp"
namespace th08::multiplayer {
bool decode_session_setup(SessionSetup& current,const std::uint32_t* words,std::size_t size) noexcept {
    if(current.started||!words||(size!=11&&size!=17))return false;
    const bool network=words[0]==3;
    if((!network&&words[0]!=1)||size!=(network?17u:11u)||words[1]<2||words[1]>3||
       words[2]>=words[1]||words[3]>4||words[4]>65535)return false;
    SessionSetup next{};
    next.player_count=words[1];next.local_player=words[2];
    next.difficulty=words[3];next.seed=words[4];
    if(network){
        next.session_id=std::uint64_t(words[5])|(std::uint64_t(words[6])<<32);
        if(!next.session_id)return false;
        for(std::uint32_t i=0;i<4;++i)next.build[i]=words[7+i];
        if(!(next.build[0]|next.build[1]|next.build[2]|next.build[3]))return false;
    }
    const std::size_t loadouts=network?11:5;
    for(std::uint32_t seat=0;seat<3;++seat){
        const auto character=words[loadouts+seat*2],shot=words[loadouts+seat*2+1];
        if(character>11||shot||(seat>=next.player_count&&character))return false;
        next.characters[seat]=character;
    }
    next.configured=true;current=next;return true;
}
std::uint32_t gameplay_contract(const SessionSetup& setup) noexcept {
    std::uint32_t hash=2166136261u;
    const auto word=[&](std::uint32_t value){for(int i=0;i<4;++i){hash^=(value>>(i*8))&255u;hash*=16777619u;}};
    // Revision 6 also binds admission to the immutable Runtime generation.
    // Two cached generations must fail HELLO even when their hand-written
    // gameplay revision happens to match.
    word(0x08000006u);word(setup.player_count);word(setup.difficulty);word(setup.seed);
    for(const auto build:setup.build)word(build);
    for(const auto character:setup.characters)word(character);
    return hash;
}
}
