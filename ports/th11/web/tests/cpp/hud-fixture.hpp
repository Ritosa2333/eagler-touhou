#pragma once
#include "../../cpp/game/Hud.hpp"
#include "render-fixture.hpp"
namespace th11::test {
struct HudFixture:HudEffects {
    AnmManager animations;
    AnmResource front,text,logo;
    GameEconomy economy;
    EnemyCommandEnvironment enemies;
    StageCompletionState completion;
    HudInput input;
    EnemyState boss{};
    std::vector<i32> sounds;
    Hud hud{animations,front,text,economy,enemies,completion,*this};
    AsciiText ascii{text};
    RecordingGraphics graphics;
    AnmRenderer renderer{graphics};
    bool hud_sound(i32 id)override{sounds.push_back(id);return true;}
};
}
