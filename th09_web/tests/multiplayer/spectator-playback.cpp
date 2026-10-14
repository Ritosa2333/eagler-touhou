#include "../../cpp/multiplayer/SpectatorPlayback.hpp"
#include <cassert>
#include <cstdio>
using th09::multiplayer::SpectatorPlayback;
int main(){
    SpectatorPlayback p;
    assert(p.Available(0,90)==0&&p.Available(0,91)==1);
    assert(p.Rate(0,900)==1&&!p.following);
    assert(p.Control(0,0,100,900)&&p.Available(100,900)==0);
    assert(!p.Control(1,99,100,900)&&p.paused);
    assert(!p.Control(1,811,100,900)&&p.paused);
    assert(p.Control(1,400,100,900)&&p.Rate(100,900)==4);
    assert(p.Available(399,900)==1);
    assert(p.Rate(400,900)==1&&!p.seeking);
    assert(p.Control(2,0,400,900)&&p.Rate(400,900)==4);
    assert(p.target==810);
    p.Boundary(500,1);assert(p.roundStart==500&&p.target==810&&p.seeking);
    assert(p.Rate(500,1000)==4&&p.target==810);
    assert(p.Rate(810,1000)==1&&!p.seeking&&p.following);
    assert(p.Rate(811,1400)==1&&p.target==810);
    assert(p.Control(2,0,811,1400)&&p.target==1310&&p.Rate(811,1400)==4);
    assert(p.Control(1,950,500,1100));p.Boundary(600,2);
    assert(p.round==2&&p.target==950&&p.Rate(600,1100)==4);
    assert(p.Available(949,1100)==1&&p.Rate(950,1100)==1);
    p.Boundary(960,0);assert(p.round==0&&p.target==960&&p.Rate(960,1100)==1);
    p.ended=true;assert(p.Limit(1000)==1000&&p.Control(1,1000,600,1000));
    assert(p.Available(999,1000)==1&&p.Rate(1000,1000)==1);
    assert(!p.Control(4,0,1000,1000));
    puts("Spectator playback: forward-only, bounded target, manual 4x, delay, pause, boundaries, exact end PASS");
}
