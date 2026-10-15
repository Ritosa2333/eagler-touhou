#include "../../cpp/multiplayer/SpectatorPlayback.hpp"
#include <cassert>
#include <cstdio>
using th09::multiplayer::SpectatorPlayback;
int main(){
    SpectatorPlayback p;
    assert(p.Available(0,90)==0&&p.Available(0,91)==1);
    assert(p.Fast(0,900)==false&&!p.following);
    assert(p.Control(0,0,100,900)&&p.Available(100,900)==0);
    assert(!p.Control(1,99,100,900)&&p.paused);
    assert(!p.Control(1,811,100,900)&&p.paused);
    assert(p.Control(1,400,100,900)&&p.Fast(100,900)==true);
    assert(p.Available(399,900)==1);
    assert(p.Fast(400,900)==false&&!p.seeking);
    assert(p.Control(2,0,400,900)&&p.Fast(400,900)==true);
    assert(p.target==810);
    p.Boundary(500,1);assert(p.roundStart==500&&p.target==810&&p.seeking);
    assert(p.Fast(500,1000)==true&&p.target==810);
    assert(!p.Fast(810,1000)&&!p.seeking&&p.following);
    // Stalls and burst delivery after reaching live never re-arm fast-forward.
    assert(!p.Fast(810,1000)&&p.Available(810,1000)==100);
    assert(!p.Fast(811,1400)&&p.target==810&&p.Available(811,1400)==499);
    // An explicit second click can request another bounded catch-up.
    assert(p.Control(2,0,811,1400)&&p.target==1310&&p.Fast(811,1400));
    assert(p.Control(2,2000,811,1400)&&p.target==1910);
    assert(p.Available(811,1400)==499); // never run unreceived input
    assert(p.Fast(1310,1600)&&p.target==1910);
    assert(!p.Fast(1910,2200)&&!p.Fast(1911,2500));
    assert(!p.Control(2,40000,1911,2500));
    assert(p.Control(1,950,500,1100));p.Boundary(600,2);
    assert(p.round==2&&p.target==950&&p.Fast(600,1100)==true);
    assert(p.Available(949,1100)==1&&p.Fast(950,1100)==false);
    p.Boundary(960,0);assert(p.round==0&&p.target==960&&p.Fast(960,1100)==false);
    p.ended=true;assert(p.Limit(1000)==1000&&p.Control(1,1000,600,1000));
    assert(p.Available(999,1000)==1&&p.Fast(1000,1000)==false);
    assert(!p.Control(4,0,1000,1000));
    puts("Spectator playback: forward-only, bounded target, unlimited forward fast-forward, delay, pause, boundaries, exact end PASS");
}
