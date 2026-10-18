#include "card.hpp"
#include "data_constants.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/player_state.hpp"
#include "../stage_completion/replay_access.hpp"
#include "../progress_state/records.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_services/services.hpp"
#include <algorithm>
#include <cmath>
#include <emmintrin.h>
namespace th20::source::card {
namespace {
double sub(double a,double b){return _mm_cvtsd_f64(_mm_sub_sd(_mm_set_sd(a),_mm_set_sd(b)));}
double add(double a,double b){return _mm_cvtsd_f64(_mm_add_sd(_mm_set_sd(a),_mm_set_sd(b)));}
double mul(double a,double b){return _mm_cvtsd_f64(_mm_mul_sd(_mm_set_sd(a),_mm_set_sd(b)));}
//543310/543320 convert to signed32; out-of-range values produce INT_MIN.
int floor_low_word(double value){
    return _mm_cvttsd_si32(_mm_set_sd(value));
}
}
void post_frame(CardInf& o){
    if(o.flags&1u){if(!(o.flags&0x40u)){o.start_time=platform::read_clock(program_entry::window_state);o.flags|=0x40u;}return;}
    if(!(o.flags&0x40u))return;
    o.last_frames=o.frames;o.elapsed=sub(platform::read_clock(program_entry::window_state),o.start_time);
    const auto remainder=std::fmod(o.elapsed,data::d_0056fe60);o.elapsed=sub(o.elapsed,remainder);
    if(remainder>=data::d_0056fe60/data::d_0056fe68)o.elapsed=add(o.elapsed,data::d_0056fe60);
    const auto whole=std::floor(o.elapsed);const int seconds=std::min(floor_low_word(whole),999);
    const int hundredths=_mm_cvttsd_si32(_mm_set_sd(mul(sub(o.elapsed,std::floor(o.elapsed)),data::d_0056fe70)));
    o.encoded_time=encode_time(seconds,hundredths);o.elapsed=0;o.flags&=~0x40u;
    const int stage=gameplay::player_state::stage(game_session::session.player_table);
    if(gameplay::controller->restart()==0)progress::write(replay::recording_stage(stage),0x1cu+o.capture_index*4u,o.encoded_time);
    else {o.encoded_time=progress::read<int>(replay::recorded_stage(stage),0x1cu+o.capture_index*4u);if(invalid_encoded_time(o.encoded_time))o.encoded_time=encode_time(999,99);}
    ++o.capture_index;
}
}
namespace th20::source::platform_window::unrecovered {void update_post_frame_game_state(){if(auto* o=card::controller())card::post_frame(*o);}}
