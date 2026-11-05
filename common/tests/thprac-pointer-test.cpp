#include <eagler/thprac/PracticePointer.hpp>
#include <cassert>
#include <cstdio>
int main() {
    eagler::thprac::PracticePointer pointers;
    // Game drag crossing either a collapsed header or an expanded window.
    assert(!pointers.route(0, 1, false));
    assert(!pointers.route(1, 1, true));
    assert(!pointers.route(1, 1, false));
    assert(!pointers.route(2, 1, true));
    // A menu drag keeps its release even after leaving the window.
    assert(pointers.route(0, 2, true));
    assert(pointers.route(1, 2, false));
    // A second game finger cannot cancel the first menu finger.
    assert(!pointers.route(0, 3, false));
    assert(!pointers.route(1, 3, true));
    assert(pointers.route(2, 2, false));
    assert(!pointers.route(1, 2, true));
    assert(!pointers.route(2, 3, true));
    assert(pointers.route(0, -1000000, true));
    pointers.reset();
    assert(!pointers.route(1, -1000000, true));
    assert(!pointers.route(2, -1000000, true));
    assert(!pointers.route(2, 8, true));
    using Route=eagler::thprac::PracticePointer::TouchRoute;
    assert(pointers.route_touch(0,10,true,true,450,260).route==Route::Pending);
    assert(pointers.route_touch(1,10,true,true,454,261).route==Route::Pending);
    auto swipe=pointers.route_touch(1,10,false,false,470,275);
    assert(swipe.route==Route::BeginGame&&swipe.x==450&&swipe.y==260);
    assert(pointers.route_touch(1,10,true,true,451,260).route==Route::Game);
    assert(pointers.route_touch(2,10,true,true,451,260).route==Route::Game);
    assert(pointers.route_touch(0,11,true,true,450,260).route==Route::Pending);
    assert(pointers.route_touch(0,12,true,false,10,10).route==Route::Ui);
    assert(pointers.route_touch(2,11,true,true,451,261).route==Route::Tap);
    assert(pointers.route_touch(2,12,false,false,20,20).route==Route::Ui);
    assert(pointers.route_touch(0,13,true,true,450,260).route==Route::Pending);
    pointers.reset();
    assert(pointers.route_touch(2,13,true,true,450,260).route==Route::Game);
    // A replacement DOWN retires stale ownership after a lost UP/CANCEL.
    assert(pointers.route(0, 9, true));
    assert(!pointers.route(0, 9, false));
    assert(!pointers.route(1, 9, true));
    // SDL's native finger identity must not alias its low 32 bits.
    const std::int64_t wide = (std::int64_t(1) << 40) + 9;
    assert(pointers.route(0, wide, true));
    assert(!pointers.route(1, 9, true));
    assert(pointers.route(2, wide, false));
    // The deferred classifier also keeps all 64 bits of a finger identity.
    assert(pointers.route_touch(0,wide,true,true,450,260).route==Route::Pending);
    assert(pointers.route_touch(1,9,true,true,470,275).route==Route::Game);
    assert(pointers.route_touch(2,wide,true,true,451,261).route==Route::Tap);
    // A replacement DOWN retires both UI and deferred ownership.
    assert(pointers.route_touch(0,20,true,true,450,260).route==Route::Pending);
    assert(pointers.route_touch(0,20,false,false,40,40).route==Route::Game);
    assert(pointers.route_touch(2,20,true,true,450,260).route==Route::Game);
    assert(pointers.route_touch(0,21,true,false,10,10).route==Route::Ui);
    assert(pointers.route_touch(0,21,true,true,450,260).route==Route::Pending);
    assert(pointers.route_touch(2,21,true,true,451,261).route==Route::Tap);
    assert(pointers.route_touch(0,22,true,true,450,260).route==Route::Pending);
    assert(pointers.route_touch(0,22,true,false,10,10).route==Route::Ui);
    assert(pointers.route_touch(2,22,false,false,20,20).route==Route::Ui);
    assert(pointers.route_touch(0,23,true,true,450,260).route==Route::Pending);
    assert(pointers.cancel(23));assert(!pointers.cancel(23));
    assert(pointers.route_touch(1,23,true,true,451,260).route==Route::Game);
    assert(pointers.route_touch(2,23,true,true,451,260).route==Route::Game);
    pointers.reset();
    std::puts("PASS trainer pointer ownership: game crossing, UI release, passive swipe/tap, 64-bit identity, replacement down, reset");
}
