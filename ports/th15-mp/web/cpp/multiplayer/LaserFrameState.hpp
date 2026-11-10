#pragma once
#include "../game/LaserScene.hpp"
namespace th15::multiplayer {
// Restores concrete laser motion/programs and their embedded VM owners. Pair
// this with AnimationState when restoring the registry graph for a full frame.
class LaserFrameState {
    struct Data;
    std::unique_ptr<Data> saved_;
public:
    mutable std::string error;
    LaserFrameState();~LaserFrameState();
    LaserFrameState(LaserFrameState&&)noexcept;
    LaserFrameState& operator=(LaserFrameState&&)noexcept;
    void discard()noexcept;
    static void append_roots(LaserScene&,const AnmRegistry&,std::vector<AnmVm*>&);
    // Full battle snapshots let AnimationState own the VM values once.
    bool capture(LaserScene&,bool own_visuals=true);
    bool restore(LaserScene&)const;
    size_t size()const noexcept;
};
}
