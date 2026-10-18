#pragma once
#include "Timer.hpp"
#include "Rng.hpp"
namespace th15 {
struct ScreenShakeSpec {i32 amplitude=0,ramp=0,hold=0,fade=0;};
struct ScreenNudgeSpec {i32 duration=0,first=0,last=0;};
struct ScreenShakeContext {bool shutdown=false,game_available=true;u32 game_flags=0;float rate=1,resolution_scale=1;};
class ScreenShake {
    Rng& random;
public:
    ScreenShakeSpec spec;Timer age;Vec2 world_offset{},screen_offset{};
    ScreenShake(Rng& rng,ScreenShakeSpec s):random(rng),spec(s){age.set(0);}
    i32 update(const ScreenShakeContext&)noexcept;
};
class ScreenNudge {
    Rng& random;
public:
    ScreenNudgeSpec spec;Timer age;Vec2 world_offset{},screen_offset{};
    ScreenNudge(Rng& rng,ScreenNudgeSpec s):random(rng),spec(s){age.set(0);}
    i32 update(const ScreenShakeContext&)noexcept;
};
}
