#include "SessionSetup.hpp"
namespace th10::multiplayer {
bool DecodeSessionSetup(SessionSetup& current,const std::uint32_t* words,
                        std::size_t size) noexcept {
    if(current.started||!words||size!=11||words[0]!=1||words[1]<2||words[1]>3||
       words[2]>=words[1]||words[3]>4||words[4]>65535)return false;
    SessionSetup next{};
    next.playerCount=words[1];next.localPlayer=words[2];
    next.difficulty=words[3];next.seed=words[4];
    for(std::uint32_t seat=0;seat<3;++seat){
        const auto character=words[5+seat*2],shot=words[6+seat*2];
        if(character>1||shot>2||(seat>=next.playerCount&&(character||shot)))return false;
        next.loadouts[seat]={character,shot};
    }
    next.configured=true;current=next;return true;
}
std::uint32_t GameplayContract(const SessionSetup& setup) noexcept {
    // Bump the version whenever deterministic title semantics change. Peer
    // identity is deliberately excluded; all peers must compute the same ABI.
    std::uint32_t hash=2166136261u;
    const auto word=[&](std::uint32_t value){for(int i=0;i<4;++i){hash^=(value>>(i*8))&255u;hash*=16777619u;}};
    word(0x10000001u);word(setup.playerCount);word(setup.difficulty);word(setup.seed);
    for(const auto& loadout:setup.loadouts){word(loadout.character);word(loadout.shot);}
    return hash;
}
}
