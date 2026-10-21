#pragma once
#include "ShotControl.hpp"
#include "ChargeSample.hpp"
namespace th09 {
class LocalChargePlanner {
    ShotControlState shadow;
    u32 generation=0;bool initialized=false;
public:
    void clear(){initialized=false;}
    void accept(const ShotControlState& live){shadow.charge=live.charge;shadow.charging_time=live.charging_time;shadow.full_charge=live.full_charge;shadow.player_state=live.player_state;}
    ChargeSample plan(const ShotControlState&,const GameInput&,bool automatic_focus,i32 side,
                      const ShotResource&,const FrameTiming&,bool advance,bool boss_available,u32 instance);
};
// Inputs and capture ownership live outside snapshots. The queued gameplay
// release belongs to the player and follows the ordinary rollback lifetime.
struct ChargeFrame {
    bool enabled=false,supplied=false,used=false,failed=false;
    u32 instance=0;
    LocalChargePlanner* owner=nullptr;
    ChargeSample input,output;
};
}
