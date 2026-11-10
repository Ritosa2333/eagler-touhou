#pragma once
#include "../game/EnemyManager.hpp"
namespace th15::multiplayer {
class EnemyFrameState {
    struct Data;
    std::unique_ptr<Data> saved_;
public:
    mutable std::string error;
    EnemyFrameState();~EnemyFrameState();
    EnemyFrameState(EnemyFrameState&&)noexcept;
    EnemyFrameState& operator=(EnemyFrameState&&)noexcept;
    void discard()noexcept;
    bool capture(EnemyManager&);
    bool restore(EnemyManager&)const;
    size_t size()const noexcept;
};
}
