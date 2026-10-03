#include "SessionSetup.hpp"
namespace th10::multiplayer {
bool DecodeSessionSetup(SessionSetup& current,const std::uint32_t* words,
                        std::size_t size) noexcept {
    if(current.started||!words||(size!=11&&size!=13&&size!=14&&size!=21))return false;
    if(!((words[0]==1&&size==11)||(words[0]==2&&size==13)||(words[0]==3&&size==14)||(words[0]==4&&size==21))||
       words[1]<2||words[1]>3||
       words[2]>=words[1]||words[3]>4||words[4]>65535)return false;
    SessionSetup next{};
    next.version=words[0];
    next.playerCount=words[1];next.localPlayer=words[2];
    next.difficulty=words[3];next.seed=words[4];
    const std::size_t loadoutBase=words[0]>=3?8:words[0]==2?7:5;
    if(words[0]>=2){
        next.sessionId=std::uint64_t(words[5])|(std::uint64_t(words[6])<<32);
        if(!next.sessionId)return false;
    }
    if(words[0]>=3){next.input_delay=words[7];if(next.input_delay>(words[0]==4?9u:8u))return false;}
    if(words[0]==4){
        if(words[14]<1||words[14]>2||words[15]>1||words[16]<1||words[16]>2||(words[15]&&next.input_delay))return false;
        next.adonis_mode=words[14];next.input_delay_auto=words[15];next.prediction_reserve=words[16];
        for(unsigned i=0;i<4;++i)next.build[i]=words[17+i];
        if(!(next.build[0]|next.build[1]|next.build[2]|next.build[3]))return false;
    }
    for(std::uint32_t seat=0;seat<3;++seat){
        const auto character=words[loadoutBase+seat*2],shot=words[loadoutBase+seat*2+1];
        if(character>1||shot>2||(seat>=next.playerCount&&(character||shot)))return false;
        next.loadouts[seat]={character,shot};
    }
    next.configured=true;current=next;return true;
}
namespace {
std::uint32_t gameplay_contract(const SessionSetup& setup,std::uint32_t version) noexcept {
    // Bump the version whenever deterministic title semantics change. Peer
    // identity is deliberately excluded; all peers must compute the same ABI.
    std::uint32_t hash=2166136261u;
    const auto word=[&](std::uint32_t value){for(int i=0;i<4;++i){hash^=(value>>(i*8))&255u;hash*=16777619u;}};
    word(version);word(setup.playerCount);word(setup.difficulty);word(setup.seed);
    for(const auto& loadout:setup.loadouts){word(loadout.character);word(loadout.shot);}
    return hash;
}
}
std::uint32_t GameplayContract(const SessionSetup& setup) noexcept {
    // Scaled P drops, rescue Power gifts, no terminal life awards and dynamic boss scaling.
    // Existing live, spectator and Replay gates must reject the old semantics.
    return gameplay_contract(setup,0x10000008u);
}
std::uint32_t LegacyGameplayContractV4(const SessionSetup& setup) noexcept {
    return gameplay_contract(setup,0x10000004u);
}
}
