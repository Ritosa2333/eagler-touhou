#include "CooperativeLifecycle.hpp"
#include <type_traits>

namespace th08::multiplayer {
namespace {
static_assert(std::is_trivially_copyable_v<CooperativeState>);
constexpr std::int64_t rescue_radius_squared=2000LL*2000LL;
void clear_progress(CooperativeSeat& seat) noexcept {seat.progress=0;seat.target=-1;}
void clear_power(CooperativeSeat& seat) noexcept {seat.power_taps=seat.power_window=0;}
bool nearby(const CooperativeSeatInput& a,const CooperativeSeatInput& b) noexcept {
    const auto x=std::int64_t(a.x)-b.x,y=std::int64_t(a.y)-b.y;
    if(x< -2000||x>2000||y< -2000||y>2000)return false;
    return x*x+y*y<=rescue_radius_squared;
}
std::int8_t recipient(const CooperativeState& state,const CooperativeFrameInput& input,std::uint8_t giver) noexcept {
    std::int8_t best=-1;bool best_spirit=false;
    for(std::uint8_t seat=0;seat<state.count;++seat){
        if(seat==giver||!nearby(input.seats[giver],input.seats[seat]))continue;
        const bool spirit=state.seats[seat].spirit;
        if(!spirit&&(!input.seats[seat].available||!input.seats[seat].can_receive||input.seats[seat].lives>=8))continue;
        if(best<0||(spirit&&!best_spirit)||
           (spirit==best_spirit&&input.seats[seat].lives<input.seats[best].lives)||
           (spirit==best_spirit&&input.seats[seat].lives==input.seats[best].lives&&seat<std::uint8_t(best))){
            best=std::int8_t(seat);best_spirit=spirit;
        }
    }
    return best;
}
std::int8_t power_recipient(const CooperativeState& state,const CooperativeFrameInput& input,std::uint8_t giver) noexcept {
    std::int8_t best=-1;
    for(std::uint8_t seat=0;seat<state.count;++seat){
        if(seat==giver||state.seats[seat].spirit||!input.seats[seat].available||
           !input.seats[seat].can_receive||input.seats[seat].power>=128||
           !nearby(input.seats[giver],input.seats[seat]))continue;
        if(best<0||input.seats[seat].power<input.seats[best].power||
           (input.seats[seat].power==input.seats[best].power&&seat<std::uint8_t(best)))best=std::int8_t(seat);
    }
    return best;
}
void emit(CooperativeTick& result,CooperativeEventKind kind,std::uint8_t giver,std::int8_t target) noexcept {
    if(result.count<4)result.events[result.count++]={kind,giver,target};
}
}
void reset(CooperativeState& state,std::uint8_t count) noexcept {
    state={};state.count=count>=2&&count<=3?count:0;
    for(auto& seat:state.seats)seat.target=-1;
}
void begin_stage(CooperativeState& state) noexcept {
    for(std::uint8_t index=0;index<state.count;++index){
        auto& seat=state.seats[index];
        clear_progress(seat);clear_power(seat);seat.waiting_for_focus_release=false;
        // A completed stage is a clean cooperative boundary.  The next native
        // Player graph is freshly initialized, so keeping the policy seat in
        // Spirit would immediately disable that new Player again and contradict
        // the shared multiplayer rule that ghosts return for the next stage.
        seat.spirit=false;
    }
    state.wipe_progress=0;state.retry_pending=false;
}
bool enter_spirit(CooperativeState& state,std::uint8_t seat,std::int8_t drift_x,std::int8_t drift_y) noexcept {
    if(seat>=state.count||state.seats[seat].spirit||!drift_x||!drift_y)return false;
    auto& current=state.seats[seat];current.spirit=true;current.drift_x=drift_x;current.drift_y=drift_y;
    clear_progress(current);clear_power(current);current.waiting_for_focus_release=false;return true;
}
void revive(CooperativeState& state,std::uint8_t seat) noexcept {
    if(seat>=state.count)return;
    auto& current=state.seats[seat];current.spirit=false;current.waiting_for_focus_release=false;
    clear_progress(current);clear_power(current);state.wipe_progress=0;state.retry_pending=false;
}
CooperativeTick advance(CooperativeState& state,const CooperativeFrameInput& input,
                        LifeItemAllocator allocate_life,PowerItemAllocator allocate_power,void* context) noexcept {
    CooperativeTick result{};if(state.count<2||state.count>3)return result;
    for(std::uint8_t giver=0;giver<state.count;++giver){
        auto& source=state.seats[giver];const auto& controls=input.seats[giver];
        if(source.spirit||!controls.available||!controls.can_give||controls.power<20){clear_power(source);}
        else{
            const auto power_target=power_recipient(state,input,giver);
            if(power_target<0)clear_power(source);
            else{
                if(source.power_window&&!--source.power_window)source.power_taps=0;
                if(controls.shoot_pressed){
                    ++source.power_taps;source.power_window=24;
                    if(source.power_taps>=5){
                        clear_power(source);
                        if(allocate_power&&allocate_power(context,giver,std::uint8_t(power_target)))
                            emit(result,CooperativeEventKind::PowerItems,giver,power_target);
                    }
                }
            }
        }
        if(source.waiting_for_focus_release){clear_progress(source);if(!controls.focus)source.waiting_for_focus_release=false;continue;}
        if(source.spirit||!controls.available||!controls.can_give){clear_progress(source);continue;}
        const auto target=recipient(state,input,giver);
        if(target<0){clear_progress(source);continue;}
        if(source.target!=target){source.target=target;source.progress=0;}
        if(source.power_taps||!controls.focus||controls.shoot){clear_progress(source);continue;}
        if(source.progress<rescue_ticks)++source.progress;
        if(source.progress<rescue_ticks||controls.lives<=0)continue;
        auto& receiver=state.seats[std::uint8_t(target)];
        if(receiver.spirit){
            revive(state,std::uint8_t(target));source.waiting_for_focus_release=true;clear_progress(source);
            emit(result,CooperativeEventKind::Revive,giver,target);
        }else{
            clear_progress(source);
            if(allocate_life&&allocate_life(context,giver,std::uint8_t(target))){
                source.waiting_for_focus_release=true;
                emit(result,CooperativeEventKind::LifeItem,giver,target);
            }
        }
    }
    bool wiped=true;for(std::uint8_t seat=0;seat<state.count;++seat)if(!state.seats[seat].spirit){wiped=false;break;}
    if(!wiped){state.wipe_progress=0;state.retry_pending=false;}
    else if(!state.retry_pending){
        if(state.wipe_progress<wipe_retry_ticks)++state.wipe_progress;
        if(state.wipe_progress==wipe_retry_ticks){state.retry_pending=true;emit(result,CooperativeEventKind::Retry,0,-1);}
    }
    return result;
}
}
