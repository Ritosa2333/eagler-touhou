#include "replay.hpp"
#include "input.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/player_state.hpp"
#include "../stage_completion/dependencies.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/frame_statistics.hpp"
#include "../platform_window/platform_window.hpp"
#include "../audio_runtime/music_stream.hpp"
#include "../audio_runtime/audio.hpp"
#include "../text_renderer/text.hpp"
#include <algorithm>
#include <stdexcept>
#include <emmintrin.h>
namespace th20::source::replay {
namespace {
auto& buttons(){return input::shared_state().slots[0];}
auto& statistics(){return *static_cast<platform_window::FrameStatistics*>(platform_window::unrecovered::scheduler_object_005c4a00);}
void clear_replay_buttons(){auto& s=buttons();s.retained_298[1]=s.retained_298[4]=s.retained_298[5]=0;}
PlaybackCursor& cursor(ReplayInf& o){if(o.active_stage<0||o.active_stage>=8)throw std::out_of_range("Replay active stage outside original cursor array");return o.playback[o.active_stage];}
}
int update_playback(ReplayInf& o){
    if(!gameplay::controller||(o.flags&2u))return 1;
    auto& c=cursor(o);if(c.frame<0){clear_replay_buttons();return 1;}
    auto& b=buttons();b.retained_298[2]=b.retained_298[1];
    if(c.frame<static_cast<int>(c.stage->frame_count)){
        const auto value=*c.input_cursor;
        if(value.current==0xffff&&value.pressed==0xffff&&value.released==0xffff){clear_replay_buttons();stage_completion::unrecovered::finish_replay();o.active_stage=-1;return 1;}
        b.retained_298[1]=value.current;b.retained_298[4]=value.pressed;b.retained_298[5]=value.released;update_input(b);o.fps=*c.fps_cursor;++c.input_cursor;if(o.frame%30==0)++c.fps_cursor;
    }else clear_replay_buttons();
    c.frame=recovered::signed_bits(static_cast<unsigned>(c.frame)+1u);o.frame=recovered::signed_bits(static_cast<unsigned>(o.frame)+1u);return 1;
}
int update_recording(ReplayInf& o){
    if(!gameplay::controller)return 1;auto& b=buttons();b.retained_298[2]=b.retained_298[1];b.retained_298[1]=b.current&0xffffu;update_input(b);
    if(program_entry::graphics_state.configuration.flags&0x100u){if(auto* physical=input::button_slot(0);physical&&(physical->retained_298[1]&1u)&&physical->retained_218[0]>9)b.retained_298[1]|=8u;}
    if(o.frame>=0){auto& chunk=*reinterpret_cast<RecordingChunk*>(o.active_chunk->value);
        if(o.frame%30==0){const float value=recovered::add32(statistics().frames_per_second,0.5f);const auto fps=value<256.f?static_cast<std::uint8_t>(_mm_cvttss_si32(_mm_set_ss(value))):std::uint8_t(255);chunk.append_fps(fps);}
        if(chunk.append(static_cast<std::uint16_t>(b.retained_298[1]),static_cast<std::uint16_t>(b.retained_298[4]),static_cast<std::uint16_t>(b.retained_298[5])))o.active_chunk=o.add_recording_chunk(gameplay::player_state::stage(game_session::session.player_table));
        o.frame=recovered::signed_bits(static_cast<unsigned>(o.frame)+1u);
    }return 1;
}
int update_fast_forward(ReplayInf& o){
    if(!gameplay::controller)return 1;
    if(o.mode==1){
        const auto* physical=input::button_slot(0);const bool held=physical&&(physical->current&0x201u);
        if(!held){if(o.fast_forward){auto& table=game_session::session.player_table;int at=gameplay::player_state::read<int>(table,0x1f0);at=std::clamp(at,0,999999999);gameplay::player_state::write(table,0x1f0,at);program_entry::thread_registry.stream->seek_seconds(static_cast<double>(at-8)/60.0);}o.fast_forward=0;return 1;}
        o.fast_forward=1;if(o.frame%8!=0)return 6;
    }return 1;
}
int draw(ReplayInf& o){
    if(gameplay::controller&&o.mode==1){auto& r=*text::renderer;r.color=o.fps<30?0xff5050ffu:o.fps<50?0xffa0a0ffu:0xffffffffu;r.write_ascii_format({383,450,0},"%3d",static_cast<unsigned>(o.fps));r.color=0xffffffffu;}return 1;
}
}
