#pragma once
#include <cstdint>

namespace th08 {
inline bool validation_custom_practice_eligible(bool enabled,bool active,bool replay,int mode,int run_stage,int stage)noexcept{
    return enabled&&active&&!replay&&mode==1&&run_stage==stage;
}
inline bool validation_scene_request_eligible(bool from_title,bool initial,std::uint32_t flags,bool custom_practice=false)noexcept{
    constexpr std::uint32_t practice=1u,demo=2u,replay=8u,game_state=0x60u,spell_practice=0x4000u;
    return from_title&&initial&&!(flags&(demo|replay|game_state|spell_practice))&&
        (!(flags&practice)||custom_practice);
}
// A prepared world keeps its ordinary player/input jobs, but never starts an
// ECL timeline. It is retired through the ordinary world-reset boundary.
class ValidationSceneGate {
    bool enabled_=false;
    bool update_started_=false,timeline_seen_=false;
    bool accepted_before_first_update_=false,first_timeline_withheld_=false;
    std::uint32_t withheld_updates_=0;
public:
    bool begin(bool ready,bool replay,bool busy,std::uint32_t enemies,
               std::uint32_t bullets,std::uint32_t lasers)noexcept{
        if(!ready||replay||busy||enemies||bullets||lasers)return false;
        enabled_=true;accepted_before_first_update_=false;return true;
    }
    // World creation may arm the gate after EnemySystem::reset and ECL setup,
    // but before the first EnemySimulation update is allowed to run.
    bool begin_before_first_update(bool ready,bool replay,bool busy,std::uint32_t enemies,
                                   std::uint32_t bullets,std::uint32_t lasers)noexcept{
        if(enabled_||update_started_||timeline_seen_||!ready||replay||busy||enemies||bullets||lasers)return false;
        enabled_=true;accepted_before_first_update_=true;return true;
    }
    void note_update_started()noexcept{update_started_=true;}
    bool allow_timeline()noexcept{
        if(!timeline_seen_){timeline_seen_=true;first_timeline_withheld_=enabled_;}
        if(!enabled_)return true;
        ++withheld_updates_;return false;
    }
    bool enabled()const noexcept{return enabled_;}
    bool update_started()const noexcept{return update_started_;}
    bool accepted_before_first_update()const noexcept{return accepted_before_first_update_;}
    bool first_timeline_withheld()const noexcept{return first_timeline_withheld_;}
    std::uint32_t withheld_updates()const noexcept{return withheld_updates_;}
};
}
