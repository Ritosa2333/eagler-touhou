#include "SessionSetup.hpp"
#include <eagler/netplay/AdonisTiming.hpp>
namespace th08::multiplayer {
bool decode_session_setup(SessionSetup& current,const std::uint32_t* words,std::size_t size) noexcept {
    if(current.started||!words||!size)return false;
    const bool adonis=words[0]==5,timed_network=words[0]==4||adonis,network=words[0]==3||timed_network;
    const std::size_t expected=adonis?20u:(timed_network?19u:(network?17u:11u));
    if((!network&&words[0]!=1)||size!=expected||words[1]<2||words[1]>3||
       words[2]>=words[1]||words[3]>4||words[4]>65535)return false;
    SessionSetup next{};
    next.version=words[0];
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
    if(timed_network){
        if(words[17]>(adonis?9u:8u)||words[18]<1||words[18]>8)return false;
        next.input_delay=words[17];next.prediction_limit=words[18];
    }
    if(adonis){
        if(words[19]<1||words[19]>2)return false;
        next.adonis_mode=words[19];
    }
    next.configured=true;current=next;return true;
}
std::uint32_t gameplay_contract(const SessionSetup& setup) noexcept {
    std::uint32_t hash=2166136261u;
    const auto word=[&](std::uint32_t value){for(int i=0;i<4;++i){hash^=(value>>(i*8))&255u;hash*=16777619u;}};
    // Revision 6 binds immutable Runtime identity; revision 7 additionally
    // binds the per-run timing policy so peers cannot silently disagree.
    word(setup.version==5?0x08000008u:setup.version==4?0x08000007u:0x08000006u);
    word(setup.player_count);word(setup.difficulty);word(setup.seed);
    for(const auto build:setup.build)word(build);
    for(const auto character:setup.characters)word(character);
    if(setup.input_delay||setup.prediction_limit!=8){
        word(0x54494d31u);word(setup.input_delay);word(setup.prediction_limit);
    }
    return Netplay::AdonisGameplayAbi(hash,Netplay::AdonisMode(setup.adonis_mode),setup.input_delay);
}
}
