#include "PurpleWall.hpp"
#include <array>
#include <cassert>
#include <cstdio>
struct Sprite {float heightPx=16;};
struct Bullet {
    int state=1;float speed=0;
    struct {float x=0,y=0;} pos;
    struct {struct {Sprite* sprite;} spriteBullet;} sprites;
};
int main() {
    Sprite sprite;std::array<Bullet,3> bullets{};
    for(auto& b:bullets)b.sprites.spriteBullet.sprite=&sprite;
    bullets[0].pos={100,0};bullets[1].pos={20,0};bullets[2].pos={1,0};bullets[2].state=5;
    for(float decision:{0.1f,0.3f,0.5f,0.95f}) {
        int draws=0;auto rng=[&](){return draws++==0?decision:0.25f;};
        float expected=0;
        if(decision<0.2f)expected=1.57079632679f-20*3.14159f/256-0.5235988f+0.25f*0.5235988f;
        else if(decision<0.4f)expected=1.57079632679f-20*3.14159f/256-1.570796f+0.25f*1.745329f;
        else if(decision<0.9f)expected=3.14159f-100*3.14159f/256-0.2617f+0.25f*0.5235988f;
        else expected=0.25f*6.28318f-3.1415926f;
        const float actual=PracticeRuntime::Purple::WallAngle(46,0,0,0,100,0,0,bullets,rng);
        assert(draws==2 && std::abs(expected-actual)<0.00001f);
    }
    int draws=0;
    const float final=PracticeRuntime::Purple::WallAngle(49,0,0,0,100,100,0,bullets,[&](){++draws;return 0.5f;});
    assert(draws==2 && std::abs(final-(1.57079632679f-100*3.14159f/256))<0.00001f);
    const float near=PracticeRuntime::Purple::WallSnipe(2,0,0,0,100,100,0);
    const float far=PracticeRuntime::Purple::WallSnipe(2,0,0,0,800,100,0);
    assert(std::abs(near-(0.5f+1.57079632679f))<0.00001f);
    assert(std::abs(far-(2+1.57079632679f))<0.00001f);
    std::puts("Purple TH06 wall branches, eligibility, two-draw sequence and snipe distance passed");
}
