#include "PlayerRespawnRules.hpp"
#include <bit>

namespace th128::player::respawn {
std::optional<EntryPlan> entryPlan(int elapsed) noexcept {
    if(elapsed<0||elapsed>60)return std::nullopt;
    const int raw=61440-elapsed*10240/60;
    return EntryPlan{raw,float(double(raw)/128.),elapsed<30?float(double(elapsed)*512./30.+64.):0,elapsed>=30,elapsed>=60};
}
std::optional<ControlPlan> postFieldControl(ControlState state) noexcept {
    ControlPlan result{state,std::nullopt,0};result.next.mode04=2;
    for(std::uint32_t index=0;index<8;++index){
        const auto interrupt=std::uint16_t(index<2?2u:index==2?std::uint32_t(state.value08)+7u:3u);
        if(state.targets[index]){
            result.requiredSlot=index;result.requiredInterrupt=std::bit_cast<std::int16_t>(interrupt);return result;
        }
        result.next.pending[index]=interrupt;
    }
    return result;
}
}
