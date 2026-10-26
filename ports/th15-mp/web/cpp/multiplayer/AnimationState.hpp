#pragma once
#include "../game/AnmRegistry.hpp"
#include <functional>

namespace th15::multiplayer {
// Exact frame restoration. Chapter retry's animation pool deliberately changes
// handles and normalizes timers; neither operation belongs in rollback.
class AnimationState {
    struct Data;
    std::unique_ptr<Data> saved_;
public:
    using RootResolver=std::function<AnmVm*(AnmVm*)>;
    using SpriteResolver=std::function<const AnmSpriteSource*(const AnmSpriteSource*)>;
    std::string error;
    AnimationState();~AnimationState();
    AnimationState(AnimationState&&)noexcept;
    AnimationState& operator=(AnimationState&&)noexcept;
    AnimationState(const AnimationState&)=delete;
    AnimationState& operator=(const AnimationState&)=delete;
    bool capture(const AnmRegistry&,const std::vector<AnmVm*>& owned_roots={});
    static bool registered_tree(const AnmRegistry&,const AnmVm&);
    // Reconstruct dynamic gameplay owners before resolving their embedded VMs.
    bool restore(AnmRegistry&,const RootResolver& roots={},const SpriteResolver& sprites={})const;
    bool ready()const noexcept;
    size_t instances()const noexcept;
    size_t retained_bytes()const noexcept;
};
}
