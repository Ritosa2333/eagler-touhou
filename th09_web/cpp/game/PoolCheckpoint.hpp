#pragma once
#include <cstddef>
namespace th09 {
struct EclVm;
class EffectManager;class AttackQueue;
// Optional before-write observer; the owner installs it only for an open
// speculative tick. No network, allocation or snapshot format lives here.
struct PoolCheckpoint {
    virtual ~PoolCheckpoint()=default;
    virtual void BeforeEnemy(EclVm&)=0;
    virtual void BeforeEffect(EffectManager&,std::size_t)=0;
    virtual void BeforeAttack(AttackQueue&,std::size_t)=0;
    virtual void BeforeBytes(void*,std::size_t)=0;
};
}
