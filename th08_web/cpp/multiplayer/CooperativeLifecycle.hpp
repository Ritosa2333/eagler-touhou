#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Cooperative lifecycle must not enter an ordinary build
#endif
#include <cstdint>

namespace th08::multiplayer {
constexpr std::uint8_t rescue_ticks=90;
constexpr std::uint16_t wipe_retry_ticks=180;
struct CooperativeSeat {
    bool spirit=false,waiting_for_focus_release=false;
    std::uint8_t progress=0,power_taps=0,power_window=0;
    std::int8_t target=-1,drift_x=1,drift_y=1;
};
// Only logical, pointer-free policy state lives here. Native player, item and
// menu owners apply the emitted events in the same seat order.
struct CooperativeState {
    std::uint8_t count=0;
    std::uint16_t wipe_progress=0;
    bool retry_pending=false;
    CooperativeSeat seats[3]{};
};
struct CooperativeSeatInput {
    // TH08 world coordinates in hundredths of a pixel.
    std::int32_t x=0,y=0,lives=0,power=0;
    bool available=false,can_give=false,can_receive=false,focus=false,shoot=false,shoot_pressed=false;
};
struct CooperativeFrameInput {CooperativeSeatInput seats[3]{};};
enum class CooperativeEventKind:std::uint8_t {Revive,LifeItem,PowerItems,Retry};
struct CooperativeEvent {CooperativeEventKind kind;std::uint8_t giver;std::int8_t target;};
struct CooperativeTick {CooperativeEvent events[4]{};std::uint8_t count=0;};
using LifeItemAllocator=bool (*)(void*,std::uint8_t,std::uint8_t) noexcept;
using PowerItemAllocator=bool (*)(void*,std::uint8_t,std::uint8_t) noexcept;

void reset(CooperativeState& state,std::uint8_t count) noexcept;
void begin_stage(CooperativeState& state) noexcept;
bool enter_spirit(CooperativeState& state,std::uint8_t seat,std::int8_t drift_x,std::int8_t drift_y) noexcept;
void revive(CooperativeState& state,std::uint8_t seat) noexcept;
CooperativeTick advance(CooperativeState& state,const CooperativeFrameInput& input,
                        LifeItemAllocator allocate_life=nullptr,PowerItemAllocator allocate_power=nullptr,
                        void* context=nullptr) noexcept;
}
