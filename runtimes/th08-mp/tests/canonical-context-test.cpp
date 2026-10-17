#include "../th08_web/cpp/multiplayer/CanonicalContext.hpp"
#include <cassert>
#include <cstdio>
#include <utility>
using namespace th08;
int main(){
    EclContext first{},second{};
    std::memcpy(&second,&first,sizeof(first));
    auto* bytes=reinterpret_cast<u8*>(&second);
    for(auto range:{std::pair{offsetof(EclContext,branched)+sizeof(second.branched),offsetof(EclContext,subroutine)},
                    std::pair{offsetof(EclContext,returned)+sizeof(second.returned),offsetof(EclContext,native_callback)}})
        for(auto i=range.first;i<range.second;++i)bytes[i]=255;
    multiplayer::NormalizeContextPadding(first);multiplayer::NormalizeContextPadding(second);
    assert(std::memcmp(&first,&second,sizeof(first))==0);
    const auto distinct=[&]{multiplayer::NormalizeContextPadding(second);assert(std::memcmp(&first,&second,sizeof(first))!=0);std::memcpy(&second,&first,sizeof(first));};
    second.branched=!first.branched;distinct();
    second.returned=!first.returned;distinct();
    second.native_callback=7;distinct();
    second.locals.integers[0]=42;distinct();
    second.locals.floats[0]=3.5f;distinct();
    puts("TH08 canonical ECL padding and state sensitivity: PASS");
}
