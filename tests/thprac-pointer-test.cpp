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
    std::puts("PASS trainer pointer ownership: game crossing, UI release, second finger, cancel");
}
