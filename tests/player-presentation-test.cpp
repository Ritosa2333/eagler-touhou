#include "../th10_web/cpp/multiplayer/PlayerPresentation.hpp"
#include <cassert>

int main(){
    using namespace th10::multiplayer;
    assert(player_proximity_alpha(0,0)==51);
    assert(player_proximity_alpha(50,0)==51);
    assert(player_proximity_alpha(75,0)==128);
    assert(player_proximity_alpha(100,0)==128);
    assert(player_proximity_alpha(0,100)==128);
    assert(clamp_player_alpha(200,255)==200);
    assert(clamp_player_alpha(200,153)==153);
    assert(clamp_player_alpha(40,51)==40);
}
