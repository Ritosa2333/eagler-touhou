#include "scoring.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
namespace th20::source::hud {
namespace {
std::uint64_t signed_low(std::uint64_t value){return static_cast<std::uint64_t>(static_cast<std::int64_t>(recovered::signed_bits(static_cast<unsigned>(value))));}
}
void update_score(FrontInf& o){
    auto& session=game_session::session;
    const auto target=gameplay::player_state::score(session.player_table.players[0]);
    if((target>>32)==0&&static_cast<unsigned>(target)<static_cast<unsigned>(o.score))o.score=target;
    if((target>>32)!=0||static_cast<unsigned>(o.score)<=static_cast<unsigned>(target)){
        const auto step=std::clamp<std::uint64_t>((target-o.score)/32,1,0x8d55e);
        std::uint64_t speed=(static_cast<std::uint64_t>(o.field_16c)<<32)|o.field_168;
        if(speed<step)speed=step;
        const auto distance=signed_low(target)-o.score;if(distance<speed)speed=distance;
        o.score+=speed;if(o.score>=signed_low(target))speed=0;
        o.field_168=static_cast<unsigned>(speed);o.field_16c=static_cast<unsigned>(speed>>32);
    }
    // Original deliberately sign-extends only the high-score low word here.
    if(signed_low(session.field_60)<o.score){
        session.field_60=static_cast<unsigned>(o.score);session.field_64=static_cast<unsigned>(o.score>>32);
        session.player_table.continue_count=std::clamp(session.player_table.continue_count,0,9);session.field_68=static_cast<unsigned>(session.player_table.continue_count);
        session.flags|=8u;
        if((session.flags&8u)==0)notify(*controller,3,0); //preserve4b8980 then4b8410 ordering
    }
}
}
