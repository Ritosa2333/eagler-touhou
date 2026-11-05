// Executable contract for the authored camera discontinuity gate.
#include "SceneCamera.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace th11;
int main(){
    SceneCamera a{},b{};
    a.position={0,0,0};b.position={0,0,0};a.direction={0,0,1};b.direction={0,0,1};a.up={0,1,0};b.up={0,1,0};
    assert(!camera_discontinuous(a,b));
    b.position={10,0,0};assert(!camera_discontinuous(a,b));
    b.position={100,0,0};assert(camera_discontinuous(a,b));
    b.position={0,0,0};b.direction={1,0,0};assert(camera_discontinuous(a,b));
    b.direction={0,0,1};b.up={0,-1,0};assert(camera_discontinuous(a,b));
    b.up={0,1,0};b.direction={0,0.1f,1};assert(!camera_discontinuous(a,b));
    std::puts("TH11 camera discontinuity contract PASS");
    return 0;
}
