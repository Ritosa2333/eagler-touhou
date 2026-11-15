#include "game/PlayerRules.hpp"
#include <cstdint>

namespace {
th128::player::RawPosition position{0,51200};
}

extern "C" {
void th128_player_reset(int horizontal,int vertical) { position={horizontal,vertical}; }
int th128_player_x(){return position.horizontal;}
int th128_player_y(){return position.vertical;}
int th128_player_move(unsigned holding,unsigned pressed,unsigned released,int present,int active,int counter,int heldCount) {
    if(holding>65535||pressed>65535||released>65535||(present!=0&&present!=1)||(active!=0&&active!=1))return 0;
    const th128::player::Input input{std::uint16_t(holding),std::uint16_t(pressed),std::uint16_t(released)};
    const th128::player::FocusContext context{present!=0,active!=0,counter,heldCount};
    position=th128::player::moveOneTick(position,input,context).position;
    return 1;
}
}
