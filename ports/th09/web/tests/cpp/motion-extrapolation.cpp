#include "../../cpp/sdl/MotionExtrapolation.hpp"
#include <cassert>
#include <limits>

int main() {
    using namespace th09;
    sdl::ShipExtrapolation ship;
    int first, second;
    const PlayfieldLimits field{{0,0},{100,100}};
    ship.observe(&first,0,0,1,0,{90,50,0});
    assert(!ship.moving());
    ship.observe(&first,0,0,2,0,{96,48,0});
    const auto halfway = ship.offset(.5,field);
    assert(halfway.x==3 && halfway.y==-1);
    const auto edge = ship.offset(2.,field);
    assert(edge.x==4 && edge.y==-2);
    const auto invalid = ship.offset(std::numeric_limits<double>::quiet_NaN(),field);
    assert(invalid.x==0 && invalid.y==0);
    ship.observe(&first,0,0,4,0,{98,46,0}); // skipped presentation
    assert(!ship.moving());
    ship.observe(&first,0,0,5,0,{7,40,0}); // teleport
    assert(!ship.moving());
    ship.observe(&first,0,0,6,3,{8,39,0}); // control state changed
    assert(!ship.moving());
    ship.observe(&first,0,1,7,3,{9,38,0}); // new round
    assert(!ship.moving());
    ship.observe(&first,1,1,8,3,{10,37,0}); // new stage
    assert(!ship.moving());
    ship.observe(&second,1,1,9,3,{11,36,0}); // new sprite owner
    assert(!ship.moving());
    ship.observe(&second,1,1,10,3,{12,35,0});
    assert(ship.moving());
    ship.reset(); // rollback correction
    ship.observe(&second,1,1,11,3,{13,34,0});
    assert(!ship.moving());
}
