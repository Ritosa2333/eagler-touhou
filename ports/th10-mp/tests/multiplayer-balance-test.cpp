#include "../th10_web/cpp/multiplayer/Balance.hpp"
#include <cassert>

int main(){
    using namespace th10;
    using namespace th10::multiplayer;
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    // Bombs retain native damage; only the current boss participant scale applies.
    assert(boss_damage(120,1)==120);
    assert(boss_damage(120,2)==90);
    assert(boss_damage(120,3)==80);
    assert(bomb_damage(120,2)==120);
    assert(bomb_damage(120,3)==120);
    assert(boss_damage(bomb_damage(120,3),3)==80);
    assert(boss_damage(0,3)==0);
    assert(rank_penalty(-1024,1)==-1024);
    assert(rank_penalty(-1024,2)==-512);
    assert(rank_penalty(-1024,3)==-341);
}
