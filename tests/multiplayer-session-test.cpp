#include "../th10_web/cpp/multiplayer/SessionSetup.hpp"
#include <cassert>
#include <cstring>
using namespace th10::multiplayer;
int main(){
    SessionSetup first{},second{};
    std::uint32_t words[]{1,3,0,2,65535,0,0,1,2,1,1};
    assert(DecodeSessionSetup(first,words,11));
    assert(first.playerCount==3&&first.loadouts[1].shot==2);
    const auto contract=GameplayContract(first);
    words[2]=2;assert(DecodeSessionSetup(second,words,11));
    assert(GameplayContract(second)==contract);
    words[10]=2;assert(DecodeSessionSetup(second,words,11));
    assert(GameplayContract(second)!=contract);
    const auto saved=first;
    words[1]=2;assert(!DecodeSessionSetup(first,words,11));
    assert(std::memcmp(&first,&saved,sizeof(first))==0);
    words[2]=0;words[9]=words[10]=0;
    assert(DecodeSessionSetup(first,words,11));
    words[8]=3;assert(!DecodeSessionSetup(first,words,11));
    words[8]=2;words[4]=65536;assert(!DecodeSessionSetup(first,words,11));
    words[4]=0;first.started=true;assert(!DecodeSessionSetup(first,words,11));
    assert(!DecodeSessionSetup(second,nullptr,11));

    SessionSetup network{};
    std::uint32_t v2[]{2,2,1,3,1234,0x55667788u,0x11223344u,0,0,1,2,0,0};
    assert(DecodeSessionSetup(network,v2,13));
    assert(network.sessionId==0x1122334455667788ull&&network.playerCount==2&&network.localPlayer==1);
    const auto networkContract=GameplayContract(network);
    network.sessionId=0;assert(GameplayContract(network)==networkContract);
    v2[5]=v2[6]=0;SessionSetup invalid{};assert(!DecodeSessionSetup(invalid,v2,13));
    // A short nonnull input must be rejected before dereferencing words[0].
    for(std::size_t count=0;count<11;++count)
        assert(!DecodeSessionSetup(invalid,reinterpret_cast<const std::uint32_t*>(1),count));
    assert(!DecodeSessionSetup(invalid,reinterpret_cast<const std::uint32_t*>(1),12));
    assert(!DecodeSessionSetup(invalid,reinterpret_cast<const std::uint32_t*>(1),14));
}
