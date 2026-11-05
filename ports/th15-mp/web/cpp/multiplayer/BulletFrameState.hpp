#pragma once
#include "../game/BulletScene.hpp"
namespace th15::multiplayer {
class BulletFrameState {
    struct Data;
    std::unique_ptr<Data> saved_;
public:
    mutable std::string error;
    BulletFrameState();~BulletFrameState();
    BulletFrameState(BulletFrameState&&)noexcept;
    BulletFrameState& operator=(BulletFrameState&&)noexcept;
    void append_roots(std::vector<AnmVm*>&)const;
    // Full battles share complex VM graphs with AnimationState; plain bullet
    // bodies use a contiguous image of their complete fixed state.
    bool capture(BulletScene&,bool own_visuals=true,const AnmRegistry* registry=nullptr);
    bool restore(BulletScene&)const;
};
}
