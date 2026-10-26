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
    static void append_roots(BulletScene&,const AnmRegistry&,std::vector<AnmVm*>&);
    bool capture(BulletScene&);
    bool restore(BulletScene&)const;
};
}
