#pragma once
#include "GameResources.hpp"
#include <memory>

namespace th11 {

struct CoreResources {
    AnmResource ascii;
    AnmResource bullet;
    AnmResource enemy;
    AnmResource front;
    AnmResource text;
    AnmResource title;
    AnmResource title_variant;
    AnmResource players[2];
    ShtResource shots[6];
    EclProgram defaults;
};

// Resource ownership for the live session. It intentionally contains
// no frame state, input or rendering callbacks, so failed loading cannot leave
// a half-created gameplay world behind.
struct StageResourceEffects {
    virtual ~StageResourceEffects()=default;
    virtual bool prepare(StageResources&){return true;}
    virtual void release(StageResources&){}
    virtual bool prepare_animation(AnmResource&){return true;}
    virtual void release_animation(AnmResource&){}
    virtual bool read_replay(const std::string&,std::vector<u8>&){return false;}
};
class GameSessionResources {
public:
    CoreResources core;
    // Stable addresses are required by live ANM VMs during the native
    // overlap between outgoing and incoming stage backgrounds.
    std::unique_ptr<StageResources> stage=std::make_unique<StageResources>();
    std::unique_ptr<StageResources> previous_stage;
    StageResourceEffects* effects=nullptr;
    u32 stage_number=0;
    std::string error;

    bool load_core(GameResources& source);
    bool load_stage(GameResources& source,u32 number);
    void retire_previous();
private:
    GameResources* core_source=nullptr;
};

}
