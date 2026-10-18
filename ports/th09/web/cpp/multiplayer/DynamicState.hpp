#pragma once
#include "../game/EffectManager.hpp"

namespace th09::multiplayer {
// Owning checkpoints for the three pool element types whose children can be
// destroyed/replaced during a speculative tick. Pool slots and resource
// objects must remain at stable addresses until every checkpoint retires.
// These are title state adapters, not a second transport or rollback core.
class EnemyState {
public:
    bool Save(const EclVm&);
    bool Restore(EclVm&) const;
    std::size_t Bytes() const;
    static u32 Fingerprint(const EclVm&);
private:
    EclVm saved;
    bool valid = false;
    static bool Copy(EclVm&, const EclVm&);
};
class EffectState {
public:
    bool Save(const EffectActor& source) { Copy(saved, source); valid=true; return true; }
    bool Restore(EffectActor& destination) const { if(!valid)return false; Copy(destination, saved); return true; }
    std::size_t Bytes() const;
    static u32 Fingerprint(const EffectActor&);
private:
    EffectActor saved;
    bool valid = false;
    static void Copy(EffectActor&, const EffectActor&);
};
class AttackStateSnapshot {
public:
    bool Save(const AttackActor&);
    bool Restore(AttackActor&) const;
    static u32 Fingerprint(const AttackActor&);
private:
    AttackActor saved;
    bool valid = false;
    static void Copy(AttackActor&, const AttackActor&);
};
}
