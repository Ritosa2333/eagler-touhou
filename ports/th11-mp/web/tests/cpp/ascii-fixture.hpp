#pragma once
#include "../../cpp/game/AsciiText.hpp"
#include "render-fixture.hpp"
namespace th11::test {
struct AsciiFixture {
    AnmResource resource;
    AsciiText text{resource};
    RecordingGraphics graphics;
    AnmRenderer renderer{graphics};
    SceneCamera full,play;
    AsciiFixture(){full.viewport={0,0,640,480,0,1};play.viewport={32,16,384,448,0,1};}
};
}
