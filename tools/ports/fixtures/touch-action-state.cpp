#include "TouchActionState.hpp"

int main(){
    using th08::TouchActionStateV1;
    TouchActionStateV1 state;
    state.publish_consumer(9,44,2,15,8,1,3,2,6,0,0,0,0,3,1,1,0);
    const auto* words=state.data();
    const std::uint32_t expected[TouchActionStateV1::word_count]={
        1,255,1,9,44,2,15,8,1,3,2,6,0,0,0,0,3,1,1,0,
    };
    for(std::uint32_t i=0;i<TouchActionStateV1::word_count;++i)if(words[i]!=expected[i])return 10+int(i);

    state.publish_paused(45,2,3);
    words=state.data();
    if(words[TouchActionStateV1::Version]!=1||words[TouchActionStateV1::ValidMask]!=0x81||
       words[TouchActionStateV1::PhaseWord]!=TouchActionStateV1::Paused||words[TouchActionStateV1::ControlFrame]!=45||
       words[TouchActionStateV1::Stage]!=2||words[TouchActionStateV1::ConsumedCurrent]!=0||
       words[TouchActionStateV1::ConsumedPrevious]!=0||words[TouchActionStateV1::BombActive]!=0||
       words[TouchActionStateV1::LifeState]!=3)return 40;
    state.reset();
    if(state.data()[TouchActionStateV1::ValidMask]!=0||state.data()[TouchActionStateV1::PhaseWord]!=TouchActionStateV1::None)return 41;
    return 0;
}
