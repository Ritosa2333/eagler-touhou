#include <eagler/netplay/CooperativeMenuInput.hpp>
#include <cassert>
#include <iostream>
int main() {
    Netplay::CooperativeMenuInput<unsigned> menu;
    auto frame = menu.Update({1,0,0}, 3);
    assert(frame.held == 1 && frame.pressed == 1);
    frame = menu.Update({1,0,0}, 3);
    assert(frame.held == 1 && frame.pressed == 0);
    frame = menu.Update({1,1,0}, 3);
    assert(frame.held == 1 && frame.pressed == 1);
    frame = menu.Update({1,1,256}, 3);
    assert(frame.held == 257 && frame.pressed == 256);
    const auto snapshot = menu;
    menu.Update({0,0,0}, 3);
    menu = snapshot;
    assert(menu.Update({1,1,256}, 3).pressed == 0);
    menu.Update({1,1,256}, 2);
    assert(menu.Update({1,1,256}, 3).pressed == 256);
    std::cout << "PASS held shoot, P2 confirm, P3 pause, rewind and seat removal\n";
}
