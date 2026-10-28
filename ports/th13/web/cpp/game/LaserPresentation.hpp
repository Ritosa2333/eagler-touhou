#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "BulletAppearance.hpp"
#include "StageSim.hpp"
#include <map>
#include <memory>

namespace th13 {
// External body/orb VMs, as in LaserInfiniteInf. Neither root joins the ANM
// registry update chain; the laser manager alone ticks them.
class LaserPresentation {
    struct Visual {
        struct Source final : AnmSpriteSource {
            i32 type = 0, color = 0;
            bool curve = false;
            i32 sprite_index(i32 parameter) const noexcept override {
                if(curve)return 526+color; // 437d50 ignores the ANM parameter
                return bullet_animation_parameter(type, color, parameter);
            }
        } source;
        AnmVm body, orb, tip;
    };
    AnmManager& animations;
    AnmRenderer& renderer;
    StageSim& sim;
    i32 bank;
    std::map<u32, std::unique_ptr<Visual>> visuals;
    bool birth(StageLaser&);
    bool update(StageLaser&);
    void retire(u32 generation);
    bool tick(AnmVm&);
    bool fail(const std::string& message);
public:
    std::string error;
    LaserPresentation(AnmManager&, AnmRenderer&, StageSim&, i32 bullet_bank);
    ~LaserPresentation();
    LaserPresentation(const LaserPresentation&) = delete;
    LaserPresentation& operator=(const LaserPresentation&) = delete;
    bool draw();
    const AnmVm* body(u32 generation) const;
    const AnmVm* orb(u32 generation) const;
    const AnmVm* tip(u32 generation) const;
};
}
